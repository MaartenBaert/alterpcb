/*
Copyright (C) 2016  The AlterPCB team
Contact: Maarten Baert <maarten-baert@hotmail.com>

This file is part of AlterPCB.

AlterPCB is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

AlterPCB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this AlterPCB.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Basics.h"

#include "GLHelper.h"

class DrawingViewer : public QGLWidget {
	Q_OBJECT

private:
	constexpr static uint32_t DOTGRID_KERNEL_SIZE = 32;

private:
	bool m_dark;

	GLvao m_gl_vao_basic;
	GLvbo m_gl_vbo_basic;
	GLvao m_gl_vao_dotgrid;
	GLvbo m_gl_vbo_dotgrid;

	GLvs m_gl_vs_basic;
	GLfs m_gl_fs_basic;
	GLsp m_gl_sp_basic;
	GLuint m_gl_uni_basic_center, m_gl_uni_basic_scale, m_gl_uni_basic_color;

	GLvs m_gl_vs_dotgrid;
	GLfs m_gl_fs_dotgrid;
	GLsp m_gl_sp_dotgrid;
	GLuint m_gl_uni_dotgrid_color, m_gl_uni_dotgrid_linescale, m_gl_uni_dotgrid_kernel;

	GLtex m_gl_tex_dotgrid_kernel;

public:
	DrawingViewer(QWidget *parent = NULL);
	~DrawingViewer();

	QSize sizeHint() const override;

protected:
	virtual void initializeGL() override;
	virtual void resizeGL(int width, int height) override;
	virtual void paintEvent(QPaintEvent *event) override;

	virtual void mouseMoveEvent(QMouseEvent *event) override;

private slots:
	void OnDark();

};
