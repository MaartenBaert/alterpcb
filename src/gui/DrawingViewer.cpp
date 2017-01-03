#include "DrawingViewer.h"

#include <chrono>
#include <iostream>

static const char* SOURCE_VS_BASIC = R"(
	#version 150 core

	in vec2 position;
	uniform vec2 center;
	uniform vec2 scale;

	void main() {
		gl_Position = vec4((position - center) * scale, 0.0f, 1.0f);
	}
)";
static const char* SOURCE_FS_BASIC = R"(
	#version 150 core

	out vec4 fragcolor;
	uniform vec4 color;

	void main() {
		fragcolor = color;
	}
)";

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

	setAttribute(Qt::WA_OpaquePaintEvent);
	setAutoFillBackground(false);
	setFocusPolicy(Qt::ClickFocus);
	setMinimumSize(100, 100);
	setMouseTracking(true);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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

	// create VAO and VBO
	m_gl_vao_basic.New();
	m_gl_vbo_basic.New();
	glBindVertexArray(m_gl_vao_basic);
	glBindBuffer(GL_ARRAY_BUFFER, m_gl_vbo_basic);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
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

}

void DrawingViewer::resizeGL(int width, int height) {
	UNUSED(width);
	UNUSED(height);
}

void DrawingViewer::paintEvent(QPaintEvent *event) {
	Q_UNUSED(event);
	makeCurrent();

	// prepare colors
	GLvec4 color_background = GLvec4(1.0, 1.0, 1.0, 1.0);
	GLvec4 color_grid1 = Premultiply(GLvec4(0.0, 0.0, 0.0, 0.1));
	GLvec4 color_grid2 = Premultiply(GLvec4(0.0, 0.0, 0.0, 0.2));
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
	real_t view_scale = 10.0;
	real_t view_xmin = view_x - width() / (2.0 * view_scale);
	real_t view_xmax = view_x + width() / (2.0 * view_scale);
	real_t view_ymin = view_y - height() / (2.0 * view_scale);
	real_t view_ymax = view_y + height() / (2.0 * view_scale);
	real_t effective_grid_step_x = 1.0; //self.effective_grid_step_x()
	real_t effective_grid_step_y = 1.0; //self.effective_grid_step_y()
	Vertex gridbox[4] = {
		{view_xmin, view_ymin}, //self.grid_invtransform(view_xmin, view_ymin)
		{view_xmax, view_ymin}, //self.grid_invtransform(view_xmax, view_ymin)
		{view_xmin, view_ymax}, //self.grid_invtransform(view_xmin, view_ymax)
		{view_xmax, view_ymax}, //self.grid_invtransform(view_xmax, view_ymax)
	};
	real_t gridbox_xmin = fmin(fmin(gridbox[0].x, gridbox[1].x), fmin(gridbox[2].x, gridbox[3].x));
	real_t gridbox_xmax = fmax(fmax(gridbox[0].x, gridbox[1].x), fmax(gridbox[2].x, gridbox[3].x));
	real_t gridbox_ymin = fmin(fmin(gridbox[0].y, gridbox[1].y), fmin(gridbox[2].y, gridbox[3].y));
	real_t gridbox_ymax = fmax(fmax(gridbox[0].y, gridbox[1].y), fmax(gridbox[2].y, gridbox[3].y));

	// synchronize to GPU to avoid high latency (at the cost of some FPS)
	glFinish();

	// draw background
	glViewport(0, 0, width(), height());
	glClearColor(color_background.x, color_background.y, color_background.z, color_background.w);
	glClear(GL_COLOR_BUFFER_BIT);

	// enable basic shader
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glUseProgram(m_gl_sp_basic);
	glUniform2fv(m_gl_uni_basic_center, 1, GLvec2(0.0f, 0.0f));
	glUniform2fv(m_gl_uni_basic_scale, 1, GLvec2(view_scale * 2.0 / (real_t) width(), view_scale * 2.0 / (real_t) height()));

	// bind VAO and VBO
	glBindVertexArray(m_gl_vao_basic);
	glBindBuffer(GL_ARRAY_BUFFER, m_gl_vbo_basic);

	// draw grid
	//auto t1 = std::chrono::high_resolution_clock::now();
	{
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
					Vertex p1 = {xx, gridbox_ymin}; //self.grid_transform(xx, gridbox_ymin)
					Vertex p2 = {xx, gridbox_ymax}; //self.grid_transform(xx, gridbox_ymax)
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
					Vertex p1 = {gridbox_xmin, yy}; //self.grid_transform(gridbox_xmin, yy)
					Vertex p2 = {gridbox_xmax, yy}; //self.grid_transform(gridbox_xmax, yy)
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
			glUniform4fv(m_gl_uni_basic_color, 1, color_grid1);
			glDrawArrays(GL_LINES, 0, ind1);
			glUniform4fv(m_gl_uni_basic_color, 1, color_grid2);
			glDrawArrays(GL_LINES, ind1, num - ind1);
		}
	}
	//auto t2 = std::chrono::high_resolution_clock::now();
	//std::cerr << "[Draw] grid = " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " µs" << std::endl;

	// unbind VAO and VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// disable basic shader
	glUseProgram(0);

	// switch to QPainter-based painting
	QPainter painter(this);
	painter.setFont(QFont("Monospace", 10));
	painter.setPen(QColor(255, 0, 0));
	painter.drawText(10, 20, "Test");

}
