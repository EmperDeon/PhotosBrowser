#include "gifLoader.h"


GifLoader::GifLoader(ImageProvider *p): prv(p) {
 g_mov = new QMovie;

	connect(g_mov, SIGNAL(frameChanged(int)), this, SLOT(nextFrame(int)), Qt::QueuedConnection);
}

void GifLoader::loadImage(QString file) {
	prv->setLoading(true);
	float scale, oscale;
	QString text;

	oscale = scale = 1.0f;
	g_mov->setFileName(file);
	g_mov->start();

	text = QString("File: %1 | %2/%3 ");
	text = text.arg(file.remove(0, file.lastIndexOf('/') + 1))
		.arg(prv->getCurrent() + 1)
		.arg(prv->getSize());

	prv->setText(text);
	prv->setScale(scale);
	prv->setOSCale(oscale);

	prv->setLoading(false);
}

void GifLoader::stop() {
 g_mov->stop();
}

void GifLoader::nextFrame(int i) {
	Q_UNUSED(i)
	QImage orig = g_mov->currentImage(), simg;

	simg = (orig.width() > prv->getWnd()->width() || orig.height() > prv->getWnd()->height()) ?
		       orig.scaled(prv->getWnd()->size(), Qt::KeepAspectRatio, Qt::FastTransformation) : orig;

 prv->setOriginal(orig);
	prv->setScaled(simg);
}
