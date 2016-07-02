#ifndef PHOTOSBROWSER_GIFLOADER_H
#define PHOTOSBROWSER_GIFLOADER_H
#include <imageProvider.h>

class ImageProvider;

class GifLoader : public QObject{
 Q_OBJECT

 ImageProvider *prv;
 QMovie *g_mov;

public slots:
	void nextFrame(int i);

public:
	GifLoader(ImageProvider *p);

	void loadImage(QString file);
 void stop();
};

#endif //PHOTOSBROWSER_GIFLOADER_H
