/*
	Copyright (c) 2017-2019 by Ilya Barykin
	Released under the MIT License.
	See the provided LICENSE.TXT file for details.
*/

#ifndef PHOTOSBROWSER_IMAGELOADER_H
#define PHOTOSBROWSER_IMAGELOADER_H

#include "Resizer.h"
#include "ImageProvider.h"


class Resize;

class ImageProvider;

class Window;

class ImageLoader : public QObject {
    Resize *resize;
    ImageProvider *prv;

    bool isBlurred = false;

public:
    explicit ImageLoader(ImageProvider *p);

    void loadImage(QString file);

    void toggleBlur() { isBlurred = !isBlurred; }

protected:
    QImage process(QImage in);
};


#endif //PHOTOSBROWSER_IMAGELOADER_H
