#ifndef PHOTOSBROWSER_RESIZER_H
#define PHOTOSBROWSER_RESIZER_H
#include <QtWidgets>

class Resize{
	int
		MW, MH, // Return Image w/h
		iW, iH, // Image w/h
		xS, yS, // x/y image margin
		xE, yE; // xS + iW

	QImage n, in;
	QPainter* out;
	bool shadow;

	QImage blurred(const QImage& image, const QRect& rect, int radius, bool alphaOnly = false);
	QColor getWColor(int i, int i1);
	QColor getHColor(int i, int i1);
 bool isVColor();
	bool isHColor();

	void drawGradV();
	void drawGradH();
	void drawGradC();

	void drawBackground();
	void blur();
	void drawShadow();
	void finalize();

public:
	Resize();
	QImage process(QImage in, int MW, int MH);

};

#endif //PHOTOSBROWSER_RESIZER_H
