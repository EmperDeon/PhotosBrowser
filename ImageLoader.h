#ifndef PHOTOSBROWSER_IMAGELOADER_H
#define PHOTOSBROWSER_IMAGELOADER_H
#include "resizer.h"
#include "imageProvider.h"

class Resize;
class ImageProvider;

class ImageLoader : public QObject{
	Q_OBJECT

 Resize *resize;
 ImageProvider *prv;

	bool isBlurred = false;

public:
	ImageLoader(ImageProvider *p);

	void loadImage(QString file);
	void toggleBlur() { isBlurred = !isBlurred; }

protected:
	QImage process(QImage in);
};



#endif //PHOTOSBROWSER_IMAGELOADER_H
