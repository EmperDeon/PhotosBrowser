/*
	Copyright (c) 2017-2019 by Ilya Barykin
	Released under the MIT License.
	See the provided LICENSE.TXT file for details.
*/

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
    QPainter *out;
    bool shadow;

public:
    Resize() = default;

    QImage process(QImage in, int MW, int MH);

private:
    QImage blurred(const QImage &image, const QRect &rect, int radius, bool alphaOnly = false);

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
};

#endif //PHOTOSBROWSER_RESIZER_H
