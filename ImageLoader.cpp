/*
	Copyright (c) 2017-2019 by Ilya Barykin
	Released under the MIT License.
	See the provided LICENSE.TXT file for details.
*/

#include "ImageLoader.h"

ImageLoader::ImageLoader(ImageProvider *p) : prv(p) {
    resize = new Resize();
}

void ImageLoader::loadImage(QString file) {
    prv->setLoading(true);

    QImage in(file), image, scaled;
    QString title;
    int wH = prv->getWnd()->height();
    int wW = prv->getWnd()->width();

    image = isBlurred ? process(in) : in;

    scaled = image.height() > wH || image.width() > wW ?
             image.scaled(prv->getWnd()->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation) :
             image;

    float s1 = float(wH) / image.height(), s2 = float(wW) / image.width(), scale, original_scale;
    scale = qMin(s1, s2);
    scale = scale > 1.0f ? 1.0f : scale;
    original_scale = scale;

    QString rat, size = "%1 ";
    if (in.width() / 16 * 9 == in.height()) {
        rat = "16/9";
    } else if (in.width() / 16 * 10 == in.height()) {
        rat = "16/10";
    } else {
        rat = QString::number(float(in.width()) / in.height(), 'f', 3);
    }

    double s = QFile(file).size();
    if (s > 1024 * 1024) {
        s /= 1024 * 1024;
        size += " Mb";
    } else if (s > 1024) {
        s /= 1024;
        size += " Kb";
    } else {
        size += " b";
    }

    file = file.remove(0, file.lastIndexOf('/') + 1);

    if (in.width() != 0) {

        title = QString("File: %1 | Ratio: %2 (%3:%4) | Size: %5 | %6/%7 ");
        title = title.arg(file.size() < 100 ? file : QStringRef(&file, 0, 100).toString())
                .arg(rat)
                .arg(in.width())
                .arg(in.height())
                .arg(size.arg(s, 4, 'f', 3, ' '))
                .arg(prv->getCurrent() + 1)
                .arg(prv->getSize());
    } else {
        title = QString("Error opening file: %1");
        title = title.arg(file);
    }

    prv->setText(title);
    prv->setOriginal(image);
    prv->setScale(scale);
    prv->setOriginalScale(original_scale);

    prv->setScaled(scaled);
    prv->setLoading(false);
}

QImage ImageLoader::process(QImage in) {
    int wW = prv->getWnd()->width(), wH = prv->getWnd()->height();

    float s1 = float(wH) / in.height(), s2 = float(wW) / in.width();
    int mw, mh;

    if (s1 < s2) {
        mh = in.height() > wH ? in.height() : wH;
        mw = int(float(wW) / wH * (mh));

    } else {
        mw = in.width() > wW ? in.width() : wW;
        mh = int(float(wH) / wW * (mw));
    }

    return resize->process(in, mw, mh);
}
