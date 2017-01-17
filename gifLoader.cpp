#include "gifLoader.h"


GifLoader::GifLoader(ImageProvider *p): prv(p) {
	g_mov = new QMovie;

	connect(g_mov, SIGNAL(frameChanged(int)), this, SLOT(nextFrame(int)));
}

void GifLoader::loadImage(QString file) {
	prv->getWnd()->resetCoord();

	QString text;

	g_mov->setFileName(file);
	g_mov->start();

	text = QString("File: %1 | %2/%3 ");
	text = text.arg(file.remove(0, file.lastIndexOf('/') + 1))
			.arg(prv->getCurrent() + 1)
			.arg(prv->getSize());

	prv->setText(text);
}

void GifLoader::stop() {
	g_mov->stop();
}

void GifLoader::nextFrame(int i) {
	Q_UNUSED(i)

	QImage orig = g_mov->currentImage(), simg;

	simg = (orig.width() > prv->getWnd()->width() || orig.height() > prv->getWnd()->height()) ?
	       orig.scaled(prv->getWnd()->size(), Qt::KeepAspectRatio, Qt::FastTransformation) : orig;

	int wH = prv->getWnd()->height();
	int wW = prv->getWnd()->width();
	float s1 = float(wH) / orig.height(), s2 = float(wW) / orig.width(), scale;
	scale = qMin(s1, s2);
	scale =  scale > 1.0f ? 1.0f : scale;

	prv->setScale(scale);
	prv->setOSCale(scale);
	prv->setOriginal(orig);
	prv->setScaled(simg);
}
