/*
	Copyright (c) 2017-2019 by Ilya Barykin
	Released under the MIT License.
	See the provided LICENSE.TXT file for details.
*/

#ifndef PHOTOSBROWSER_GIFLOADER_H
#define PHOTOSBROWSER_GIFLOADER_H

#include <ImageProvider.h>


class ImageProvider;

class GifLoader : public QObject {
    ImageProvider *prv;
    QMovie *g_mov;

public:
    explicit GifLoader(ImageProvider *p);

    void nextFrame(int i);

    void loadImage(QString file);

    void stop();
};

#endif //PHOTOSBROWSER_GIFLOADER_H
