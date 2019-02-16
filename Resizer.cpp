/*
	Copyright (c) 2017-2019 by Ilya Barykin
	Released under the MIT License.
	See the provided LICENSE.TXT file for details.
*/

#include "Resizer.h"

#define BLUR_COUNT 10

QColor Resize::getWColor(int i, int i1) {
	int r = 0, g = 0, b = 0;
	int stx, enx, sty, eny, pc = 0;
	if (i <= 0 || i >= in.width()) return {};

	stx = i - 8;
	enx = i + 8;
	sty = (i1 == 0 ? 0 : in.height() - BLUR_COUNT);
	eny = (i1 == 0 ? BLUR_COUNT : in.height());

	for ( int tx = stx; tx < enx; tx++ )
		for ( int ty = sty; ty < eny; ty++ )
			if (tx >= 0 && tx < in.width() && ty >= 0 && ty < in.height()) {
				QColor c = in.pixel(tx, ty);
				r += c.red();
				g += c.green();
				b += c.blue();
				pc++;
			}

	return {r / pc, g / pc, b / pc};
}

QColor Resize::getHColor(int i, int i1) {
	int r = 0, g = 0, b = 0;
	int stx, enx, sty, eny, pc = 0;
	if (i < 0 || i >= in.height()) return {};

	stx = (i1 == 0 ? 0 : in.width() - BLUR_COUNT);
	enx = (i1 == 0 ? BLUR_COUNT : in.width());
	sty = i - 8;
	eny = i + 8;

	for ( int tx = stx; tx < enx; tx++ )
		for ( int ty = sty; ty < eny; ty++ )
			if (tx >= 0 && tx < in.width() && ty >= 0 && ty < in.height()) {
				QColor c = in.pixel(tx, ty);
				r += c.red();
				g += c.green();
				b += c.blue();
				pc++;
			}

	return QColor(r / pc, g / pc, b / pc);
}

bool Resize::isVColor() {
	int tx = in.width() - 1;
	QRgb r1 = in.pixel(0, 0), r2 = in.pixel(tx, 0);

	for ( int ty = 0; ty < in.height(); ty++ )
		if (r1 != in.pixel(0, ty) || r2 != in.pixel(tx, ty))
			return false;

	return true;
}

bool Resize::isHColor() {
	int ty = in.height() - 1;
	QRgb r1 = in.pixel(0, 0), r2 = in.pixel(0, ty);

	for ( int tx = 0; tx < in.height(); tx++ )
		if (r1 != in.pixel(tx, 0) || r2 != in.pixel(tx, ty))
			return false;

	return true;
}

void Resize::drawGradV() {
	int xS = (MW - in.width()) / 2, // Line Start
		xE = xS + in.width(), // Line End
		lW = 2, // Line width
		ti; // Temp int

	out->setPen(Qt::NoPen);
	out->drawImage(xS, 100, in);


	if (isVColor()) {
		out->setBrush(QBrush(QColor(in.pixel(0, 0))));
		out->drawRect(0, 0, xS, in.height() + 200);

		out->setBrush(QBrush(QColor(in.pixel(in.width() - 1, 0))));
		out->drawRect(xE, 0, MW, in.height() + 200);

		shadow = false;

	} else {
		for ( int i = 0; i < in.height(); i++ ) {// Left - Right
			ti = i + 100;

			out->setPen(QPen(QBrush(getHColor(i, 0)), lW));
			out->drawLine(0, ti, xS, ti);

			out->setPen(QPen(QBrush(getHColor(i, 1)), lW));
			out->drawLine(xE, ti, MW, ti);
		}

		shadow = true;
	}
}

void Resize::drawGradH() {
	int lW = 2, // Line width
		ti; // Temp int

	out->setPen(Qt::NoPen);
	out->drawImage(100, yS, in);


	if (isHColor()) {
		out->setBrush(QBrush(QColor(in.pixel(0, 0))));
		out->drawRect(0, 0, in.width() + 200, yS);

		out->setBrush(QBrush(QColor(in.pixel(in.width() - 1, 0))));
		out->drawRect(0, yE, in.width() + 200, MH);

		shadow = false;

	} else {
		for ( int i = 0; i < in.height(); i++ ) {// Left - Right
			ti = i + 100;

			out->setPen(QPen(QBrush(getWColor(i, 0)), lW));
			out->drawLine(ti, 0, ti, yS);

			out->setPen(QPen(QBrush(getWColor(i, 1)), lW));
			out->drawLine(ti, yE, ti, MH);
		}

		shadow = true;
	}
}

void Resize::drawGradC() {
	int ti, pi;

	out->setPen(Qt::NoPen);
	out->drawImage(xS, yS, in);

	// Gradients Center
	int lW = 2;

	for ( int i = xS; i < iW - xS; i++ ) {// Top - Bottom
		ti = xS + i;

		out->setPen(QPen(QBrush(getWColor(i, 0)), lW));
		out->drawLine(ti, 0, ti, yS);

		out->setPen(QPen(QBrush(getWColor(i, 1)), lW));
		out->drawLine(ti, yE, ti, MH);
	}

	for ( int i = yS; i < iH - yS; i++ ) {// Left - Right
		ti = yS + i;

		out->setPen(QPen(QBrush(getHColor(i, 0)), lW));
		out->drawLine(0, ti, xS, ti);

		out->setPen(QPen(QBrush(getHColor(i, 1)), lW));
		out->drawLine(xE, ti, MW, ti);
	}
	// Gradient center


	// Gradient angles
	for ( int i = 0; i < xS; i++ ) {
		ti = xS + i;
		pi = i * 2;

		out->setPen(QPen(QBrush(getWColor(i, 0)), lW));
		out->drawLine(pi, 0, ti, yS);

		out->setPen(QPen(QBrush(getWColor(iW - ti, 0)), lW));
		out->drawLine(iW + pi, 0, iW + i, yS);

		out->setPen(QPen(QBrush(getWColor(i, 1)), lW));
		out->drawLine(pi, MH - 1, ti, yE);

		out->setPen(QPen(QBrush(getWColor(iW - ti, 1)), lW));
		out->drawLine(iW + pi, MH - 1, iW + i, yE);
	}

	for ( int i = 0; i < yS; i++ ) {
		ti = yS + i;
		pi = i * 2;

		out->setPen(QPen(QBrush(getHColor(i, 0)), lW));
		out->drawLine(0, pi, xS, ti);

		out->setPen(QPen(QBrush(getHColor(iH - ti, 0)), lW));
		out->drawLine(0, iH + pi, xS, iH + i);

		out->setPen(QPen(QBrush(getHColor(i, 1)), lW));
		out->drawLine(MW - 1, pi, xE, ti);

		out->setPen(QPen(QBrush(getHColor(iH - ti, 1)), lW));
		out->drawLine(MW - 1, iH + pi, xE, iH + i);
	}
}

QImage Resize::process(QImage in, int MW, int MH) {
	if (in.isNull()) return in;
	if (in.width() == MW && in.height() == MH) return in;

	this->MW = MW + 200;
	this->MH = MH + 200;
	this->in = in;
	this->n = QImage(this->MW, this->MH, QImage::Format_RGB32);

	this->out = new QPainter(&n);
	this->shadow = true;

	iW = this->in.width();
	iH = this->in.height();
	xS = (this->MW - iW) / 2;
	yS = (this->MH - iH) / 2;
	xE = xS + iW;
	yE = yS + iH;

	drawBackground();
	if (shadow)
		 blur();
	if (shadow)
		drawShadow();
	finalize();

	return n;
}

void Resize::drawBackground() {
// Fill background before gradients
	int fW = MW / 2, fH = MH / 2;
	out->setBrush(QBrush(QColor(in.pixel(0, 0))));
	out->drawRect(0, 0, fW, fH);

	out->setBrush(QBrush(QColor(in.pixel(0, iH - 1))));
	out->drawRect(0, fH, fW, fH);

	out->setBrush(QBrush(QColor(in.pixel(iW - 1, 0))));
	out->drawRect(fW, 0, fW, fH);

	out->setBrush(QBrush(QColor(in.pixel(iW - 1, iH - 1))));
	out->drawRect(fW, fH, fW, fH);

	// 'Gradients'
	if (iH == MH - 200) {// Vertical-only
		drawGradV();

	} else if (iH == MW - 200) {// Horizontal-only
		drawGradH();

	} else {// Vertical, Horizontal and Corners
		drawGradC();

	}
}

QImage Resize::blurred(const QImage &image, const QRect &rect, int radius, bool alphaOnly) {
	int tab[] = {14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2};
	int alpha = (radius < 1) ? 16 : (radius > 17) ? 1 : tab[radius - 1];

	QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
	int r1 = rect.top();
	int r2 = rect.bottom();
	int c1 = rect.left();
	int c2 = rect.right();

	int bpl = result.bytesPerLine();
	int rgba[4];
	unsigned char *p;

	int i1 = 0;
	int i2 = 3;

	if (alphaOnly)
		i1 = i2 = (QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3);

	for (int col = c1; col <= c2; col++) {
		p = result.scanLine(r1) + col * 4;
		for (int i = i1; i <= i2; i++)
			rgba[i] = p[i] << 4;

		p += bpl;
		for (int j = r1; j < r2; j++, p += bpl)
			for (int i = i1; i <= i2; i++)
				p[i] = ((rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4);
	}

	for (int row = r1; row <= r2; row++) {
		p = result.scanLine(row) + c1 * 4;
		for (int i = i1; i <= i2; i++)
			rgba[i] = p[i] << 4;

		p += 4;
		for (int j = c1; j < c2; j++, p += 4)
			for (int i = i1; i <= i2; i++)
				p[i] = ((rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4);
	}

	for (int col = c1; col <= c2; col++) {
		p = result.scanLine(r2) + col * 4;
		for (int i = i1; i <= i2; i++)
			rgba[i] = p[i] << 4;

		p -= bpl;
		for (int j = r1; j < r2; j++, p -= bpl)
			for (int i = i1; i <= i2; i++)
				p[i] = ((rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4);
	}

	for (int row = r1; row <= r2; row++) {
		p = result.scanLine(row) + c2 * 4;
		for (int i = i1; i <= i2; i++)
			rgba[i] = p[i] << 4;

		p -= 4;
		for (int j = c1; j < c2; j++, p -= 4)
			for (int i = i1; i <= i2; i++)
				p[i] = ((rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4);
	}

	return result;
}

void Resize::blur() {
	auto *eb = new QGraphicsBlurEffect;
	eb->setBlurRadius(20);
	eb->setBlurHints(QGraphicsBlurEffect::QualityHint);

	QGraphicsScene scene;
	QGraphicsPixmapItem item;
	item.setPixmap(QPixmap::fromImage(n));
	item.setGraphicsEffect(eb);
	scene.addItem(&item);
	scene.render(out, QRectF(), QRectF());
}

void Resize::drawShadow() {
	auto *e = new QGraphicsDropShadowEffect;
	QGraphicsScene scene;
	QGraphicsPixmapItem item;
	e->setBlurRadius(100);
	e->setOffset(-1, -1);
	e->setColor(Qt::black);

	item.setPixmap(QPixmap::fromImage(in));
	item.setGraphicsEffect(e);
	scene.addItem(&item);
	int ex = 150;
	scene.render(out, QRectF(xS - ex, yS - ex, iW + ex * 2, iH + ex * 2), QRectF(-ex, -ex, iW + ex * 2, iH + ex * 2));
}

void Resize::finalize() {
	xS = (MW - iW) / 2, yS = (MH - iH) / 2;
	out->drawImage(xS, yS, in);
	out->end();

	n = n.copy(100, 100, MW - 200, MH - 200);
}









