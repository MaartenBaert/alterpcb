#include "DrawingViewer.h"

#include <chrono>
#include <iostream>

static const char* SOURCE_VS_BASIC = R"(
	#version 130

	in vec2 position;
	uniform vec2 center;
	uniform vec2 scale;

	void main() {
		gl_Position = vec4((position - center) * scale, 0.0f, 1.0f);
	}
)";
static const char* SOURCE_FS_BASIC = R"(
	#version 130

	out vec4 fragcolor;
	uniform vec4 color;

	void main() {
		fragcolor = color;
	}
)";

static const char* SOURCE_VS_DOTGRID = R"(
	#version 130

	in vec2 position;
	in vec2 gridposition;
	out vec2 gridposition2;

	void main() {
		gl_Position = vec4(position, 0.0f, 1.0f);
		gridposition2 = gridposition;
	}
)";
static const char* SOURCE_FS_DOTGRID = R"(
	#version 130

	in vec2 gridposition2;
	out vec4 fragcolor;
	uniform vec2 linescale;
	uniform vec4 color;
	uniform sampler2D kernel;

	void main() {
		float fx = linescale.x * (gridposition2.x - round(gridposition2.x));
		float fy = linescale.y * (gridposition2.y - round(gridposition2.y));
		float alpha = texture2D(kernel, vec2(0.5f + 0.5f * fx, 0.5f + 0.5f * fy)).r;
		fragcolor = color * alpha;
	}
)";

//TODO// this is just for testing
Vertex grid_origin = {10.0, 5.0};
real_t grid_angle = 0.2;

Vertex TransformCoordinate(const Vertex &v, const Vertex &origin, real_t angle) {
	real_t angle_sin = sin(ToRadians(angle)), angle_cos = cos(ToRadians(angle));
	return Vertex(
		origin.x + v.x * angle_cos - v.y * angle_sin,
		origin.y + v.y * angle_cos + v.x * angle_sin
	);
}

Vertex InvTransformCoordinate(const Vertex &v, const Vertex &origin, real_t angle) {
	real_t angle_sin = sin(ToRadians(angle)), angle_cos = cos(ToRadians(angle));
	return Vertex(
		(v.x - origin.x) * angle_cos + (v.y - origin.y) * angle_sin,
		(v.y - origin.y) * angle_cos - (v.x - origin.x) * angle_sin
	);
}

Vertex TransformGrid(const Vertex &v) {
	return TransformCoordinate(v, grid_origin, grid_angle);
}

Vertex InvTransformGrid(const Vertex &v) {
	return InvTransformCoordinate(v, grid_origin, grid_angle);
}

struct Vertex_DotGrid {
	GLvec2 position, gridposition;
};

static QGLFormat MakeGLFormat() {
	QGLFormat f;
	f.setAlpha(true);
	f.setDepth(false);
	f.setStencil(false);
	f.setDoubleBuffer(true);
	f.setSwapInterval(0);
	return f;
}

DrawingViewer::DrawingViewer(QWidget *parent)
	: QGLWidget(MakeGLFormat(), parent) {

	m_dark = false;

	setAttribute(Qt::WA_OpaquePaintEvent);
	setAutoFillBackground(false);
	setFocusPolicy(Qt::ClickFocus);
	setMinimumSize(100, 100);
	setMouseTracking(true);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	new QShortcut(QKeySequence("Ctrl+D"), this, SLOT(OnDark()));

}

DrawingViewer::~DrawingViewer() {
	makeCurrent(); // needed for proper destruction of GL objects
}

QSize DrawingViewer::sizeHint() const {
	return QSize(600, 600);
}

void DrawingViewer::initializeGL() {

	// initialize OpenGL
	GLInit();
	GLRegisterDebugCallback();

	// create basic VAO and VBO
	m_gl_vao_basic.New();
	m_gl_vbo_basic.New();
	glBindVertexArray(m_gl_vao_basic);
	glBindBuffer(GL_ARRAY_BUFFER, m_gl_vbo_basic);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// create dotgrid VAO and VBO
	m_gl_vao_dotgrid.New();
	m_gl_vbo_dotgrid.New();
	glBindVertexArray(m_gl_vao_dotgrid);
	glBindBuffer(GL_ARRAY_BUFFER, m_gl_vbo_dotgrid);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_DotGrid), (void*) offsetof(Vertex_DotGrid, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_DotGrid), (void*) offsetof(Vertex_DotGrid, gridposition));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// compile basic shader
	CompileVS(m_gl_vs_basic, SOURCE_VS_BASIC);
	CompileFS(m_gl_fs_basic, SOURCE_FS_BASIC);
	LinkSP(m_gl_sp_basic, m_gl_vs_basic, 0, m_gl_fs_basic, {"position"}, {"fragcolor"}, {
		{m_gl_uni_basic_center, "center"},
		{m_gl_uni_basic_scale, "scale"},
		{m_gl_uni_basic_color, "color"},
	});

	// compile dotgrid shader
	CompileVS(m_gl_vs_dotgrid, SOURCE_VS_DOTGRID);
	CompileFS(m_gl_fs_dotgrid, SOURCE_FS_DOTGRID);
	LinkSP(m_gl_sp_dotgrid, m_gl_vs_dotgrid, 0, m_gl_fs_dotgrid, {"position", "gridposition"}, {"fragcolor"}, {
		{m_gl_uni_dotgrid_linescale, "linescale"},
		{m_gl_uni_dotgrid_color, "color"},
		{m_gl_uni_dotgrid_kernel, "kernel"},
	});

	// generate dotgrid kernel texture
	{
		uint32_t n = DOTGRID_KERNEL_SIZE;
		float s = 2.0f / (float) (n - 1);
		std::vector<float> kernel(n * n);
		for(uint32_t j = 0; j < n; ++j) {
			float y = (float) j * s - 1.0f;
			float *row = kernel.data() + j * n;
			for(uint32_t i = 0; i < n; ++i) {
				float x = (float) i * s - 1.0f;
				float r = hypot(x, y);
				row[i] = fmax(0.0f, 1.0f - r); // circular dots
				//row[i] = fmax(1.0f - fabs(x), 1.0f - fabs(y)); // grid lines
			}
		}
		m_gl_tex_dotgrid_kernel.New();
		glBindTexture(GL_TEXTURE_2D, m_gl_tex_dotgrid_kernel);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, DOTGRID_KERNEL_SIZE, DOTGRID_KERNEL_SIZE, 0, GL_RED, GL_FLOAT, kernel.data());
	}

}

void DrawingViewer::resizeGL(int width, int height) {
	UNUSED(width);
	UNUSED(height);
}

void DrawingViewer::paintEvent(QPaintEvent *event) {
	Q_UNUSED(event);
	makeCurrent();

	// prepare colors
	Color color_background = LumaInvert(Color(1.0f, 1.0f, 1.0f, 1.0f), m_dark); // glClear takes sRGB values
	Color color_grid1 = Premultiply(FromSRGB(LumaInvert(Color(0.0f, 0.0f, 0.0f, 0.1f), m_dark), m_dark));
	Color color_grid2 = Premultiply(FromSRGB(LumaInvert(Color(0.0f, 0.0f, 0.0f, 0.2f), m_dark), m_dark));
	Color color_grid3 = Premultiply(FromSRGB(LumaInvert(Color(0.0f, 0.0f, 0.0f, 0.6f), m_dark), m_dark));
	/*QColor color_suppress = QColor::fromRgbF(1.0, 1.0, 1.0, 0.5);
	QColor color_selectionpen = QColor::fromRgbF(0.0, 0.0, 0.0, 0.2);
	QColor color_selectionbrush = QColor::fromRgbF(0.0, 0.0, 0.0, 0.1);
	QColor color_handlepen = QColor::fromRgbF(0.2, 0.2, 0.2);
	QColor color_handlebrush = QColor::fromRgbF(0.8, 0.8, 0.8);
	QColor color_cursor = QColor::fromRgbF(0.0, 0.0, 0.0, 0.3);*/

	// prepare view
	//TODO// replace this with actual view data
	real_t view_x = 0.0;
	real_t view_y = 0.0;
	real_t view_scale = 5.0;
	real_t view_xmin = view_x - width() / (2.0 * view_scale);
	real_t view_xmax = view_x + width() / (2.0 * view_scale);
	real_t view_ymin = view_y - height() / (2.0 * view_scale);
	real_t view_ymax = view_y + height() / (2.0 * view_scale);
	real_t effective_grid_step_x = 1.0; //self.effective_grid_step_x()
	real_t effective_grid_step_y = 1.0; //self.effective_grid_step_y()
	Vertex gridbox[4] = {
		InvTransformGrid(Vertex(view_xmin, view_ymin)),
		InvTransformGrid(Vertex(view_xmax, view_ymin)),
		InvTransformGrid(Vertex(view_xmin, view_ymax)),
		InvTransformGrid(Vertex(view_xmax, view_ymax)),
	};
	real_t gridbox_xmin = fmin(fmin(gridbox[0].x, gridbox[1].x), fmin(gridbox[2].x, gridbox[3].x));
	real_t gridbox_xmax = fmax(fmax(gridbox[0].x, gridbox[1].x), fmax(gridbox[2].x, gridbox[3].x));
	real_t gridbox_ymin = fmin(fmin(gridbox[0].y, gridbox[1].y), fmin(gridbox[2].y, gridbox[3].y));
	real_t gridbox_ymax = fmax(fmax(gridbox[0].y, gridbox[1].y), fmax(gridbox[2].y, gridbox[3].y));

	// synchronize to GPU to avoid high latency (at the cost of some FPS)
	glFinish();

	// draw background
	glViewport(0, 0, width(), height());
	glClearColor(color_background.r, color_background.g, color_background.b, color_background.a);
	glClear(GL_COLOR_BUFFER_BIT);

	// set blend mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_FRAMEBUFFER_SRGB);

	// draw grid
	//auto t1 = std::chrono::high_resolution_clock::now();
	bool linegrid = false;
	if(linegrid) {

		// enable basic shader
		glUseProgram(m_gl_sp_basic);
		glUniform2fv(m_gl_uni_basic_center, 1, GLvec2(0.0f, 0.0f));
		glUniform2fv(m_gl_uni_basic_scale, 1, GLvec2(view_scale * 2.0 / (real_t) width(), view_scale * 2.0 / (real_t) height()));

		// bind VAO and VBO
		glBindVertexArray(m_gl_vao_basic);
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_vbo_basic);

		int32_t xi_min = lrint(gridbox_xmin / effective_grid_step_x + 0.4);
		int32_t xi_max = lrint(gridbox_xmax / effective_grid_step_x - 0.4);
		int32_t yi_min = lrint(gridbox_ymin / effective_grid_step_y + 0.4);
		int32_t yi_max = lrint(gridbox_ymax / effective_grid_step_y - 0.4);
		index_t num = 2 * (std::max(0, xi_max - xi_min + 1) + std::max(0, yi_max - yi_min + 1));
		if(num != 0) {
			index_t ind1 = 0, ind2 = num;
			glBufferData(GL_ARRAY_BUFFER, num * sizeof(GLvec2), NULL, GL_STREAM_DRAW);
			GLvec2 *data = (GLvec2*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			if(data != NULL) {
				for(int32_t xi = xi_min; xi <= xi_max; ++xi) {
					real_t xx = (real_t) xi * effective_grid_step_x;
					Vertex p1 = TransformGrid(Vertex(xx, gridbox_ymin)); //self.grid_transform(xx, gridbox_ymin)
					Vertex p2 = TransformGrid(Vertex(xx, gridbox_ymax)); //self.grid_transform(xx, gridbox_ymax)
					if(xi % 10 == 0) {
						data[--ind2] = GLvec2(p1);
						data[--ind2] = GLvec2(p2);
					} else {
						data[ind1++] = GLvec2(p1);
						data[ind1++] = GLvec2(p2);
					}
				}
				for(int32_t yi = yi_min; yi <= yi_max; ++yi) {
					real_t yy = (real_t) yi * effective_grid_step_y;
					Vertex p1 = TransformGrid(Vertex(gridbox_xmin, yy)); //self.grid_transform(gridbox_xmin, yy)
					Vertex p2 = TransformGrid(Vertex(gridbox_xmax, yy)); //self.grid_transform(gridbox_xmax, yy)
					if(yi % 10 == 0) {
						data[--ind2] = GLvec2(p1);
						data[--ind2] = GLvec2(p2);
					} else {
						data[ind1++] = GLvec2(p1);
						data[ind1++] = GLvec2(p2);
					}
				}
				assert(ind1 == ind2);
				glUnmapBuffer(GL_ARRAY_BUFFER);
			}
			glUniform4fv(m_gl_uni_basic_color, 1, GLvec4(color_grid1));
			glDrawArrays(GL_LINES, 0, ind1);
			glUniform4fv(m_gl_uni_basic_color, 1, GLvec4(color_grid2));
			glDrawArrays(GL_LINES, ind1, num - ind1);
		}

		// unbind VAO and VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// disable basic shader
		glUseProgram(0);

	} else {

		// bind phosphor kernel texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_gl_tex_dotgrid_kernel);

		// enable dotgrid shader
		glUseProgram(m_gl_sp_dotgrid);
		glUniform2fv(m_gl_uni_dotgrid_linescale, 1, GLvec2(effective_grid_step_x * view_scale, effective_grid_step_y * view_scale));
		glUniform4fv(m_gl_uni_dotgrid_color, 1, GLvec4(color_grid3));
		glUniform1i(m_gl_uni_dotgrid_kernel, 0);

		// bind VAO and VBO
		glBindVertexArray(m_gl_vao_dotgrid);
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_vbo_dotgrid);

		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex_DotGrid), NULL, GL_STREAM_DRAW);
		Vertex_DotGrid *data = (Vertex_DotGrid*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		if(data != NULL) {
			data[0].position = GLvec2(-1.0f, -1.0f);
			data[0].gridposition = gridbox[0];
			data[1].position = GLvec2(1.0f, -1.0f);
			data[1].gridposition = gridbox[1];
			data[2].position = GLvec2(1.0f, 1.0f);
			data[2].gridposition = gridbox[3];
			data[3].position = GLvec2(1.0f, 1.0f);
			data[3].gridposition = gridbox[3];
			data[4].position = GLvec2(-1.0f, 1.0f);
			data[4].gridposition = gridbox[2];
			data[5].position = GLvec2(-1.0f, -1.0f);
			data[5].gridposition = gridbox[0];
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// unbind VAO and VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// disable basic shader
		glUseProgram(0);

	}
	//auto t2 = std::chrono::high_resolution_clock::now();
	//std::cerr << "[Draw] grid = " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " µs" << std::endl;

	// switch to QPainter-based painting
	QPainter painter(this);
	painter.setFont(QFont("Monospace", 10));
	painter.setPen(Color(1.0f, 0.0f, 0.0f));
	painter.drawText(QRect(5, 5, 100, 20), Qt::AlignLeft | Qt::AlignVCenter, "Test");

}

void DrawingViewer::OnDark() {
	m_dark = !m_dark;
	update();
}
