#include "imageProvider.h"
#include <QtConcurrent/QtConcurrent>

ImageProvider::ImageProvider(PsWnd *w, QString dir) : wnd(w) {
	io = new PIO(this);
	files = new QStringList;
	imgLoader = new ImageLoader(this);
	gifLoader = new GifLoader(this);
	mutex = new QMutex;

	connect(this, &ImageProvider::titleChanged, wnd, &PsWnd::titleChanged, Qt::QueuedConnection);

	QFile f(QApplication::applicationDirPath() + "/photos_last.json");
	if(f.open(QFile::ReadOnly)){
		lastPicture = QJsonDocument::fromJson(f.readAll()).object();
		f.close();
	}

	if (dir == "") {
		io->setNewCurrentDir();
	} else {
		io->setCurrentDir(dir);
		loadDir();
	}
}

void ImageProvider::keyPressEvent(int i) {
	switch (i) {
		case 16777234: prev(1); break;
		case 16777236: next(1); break;
		case 91      : prev(100); break;
		case 93      : next(100); break;
		case 16777223: del(); break;
		case 32      : cop(); break;
		case 69      : exp(); break;
		case 66      : toggleBlur(); break;
		case 44      : loadNewDir(-1); break;
		case 46      : loadNewDir(1); break;
		case 71      :
		case 1055    : goTo(); break;
		default      : qDebug() << i; break;
	}
}

void ImageProvider::closeEvent() {
	if(files->size() != 0){
		lastPicture.insert(getDir(files->value(0)), current);
	}

	saveLastPicture();
}

void ImageProvider::saveLastPicture() const {
	QFile f(QCoreApplication::applicationDirPath() + "/photos_last.json");
	if(f.open(QIODevice::WriteOnly)){
		f.write(QJsonDocument(lastPicture).toJson());
		f.close();
	}
}


// Images loading/viewing
void ImageProvider::next(int i) {
	if(!isLoading) {
		current = current < files->size() - i ? current + i : 0;
		loadImage();
	}
}

void ImageProvider::prev(int i) {
	if(!isLoading) {
		current = current >= i ? current - i : files->size() - i;
		current = current >= 0 ? current : files->size()-1;
		loadImage();
	}
}

void ImageProvider::loadImage(bool rel) {
	QString file = files->value(current);

	if (files->size() > 0) {
		if (!rel && files->size() != 0) {
			lastPicture.insert(getDir(files->value(0)), current);
			saveLastPicture();
		}

		gifLoader->stop();
		if (file.endsWith(".gif")) {
			gifLoader->loadImage(file);

		} else {
			if (!isLoading)
				QtConcurrent::run(imgLoader, &ImageLoader::loadImage, file);
		}
	} else {
		setText("");
		QImage noImage;
		setOriginal(noImage);
		setScale(1);
		setOSCale(1);

		setScaled(noImage);
	}
}

void ImageProvider::goTo() {
	current = QInputDialog::getInt(wnd, "Go to", "Go to:", current, 0, files->size()) - 1;
	loadImage();
}

void ImageProvider::toggleBlur() {
	if(loaderType == 0){
		imgLoader->toggleBlur();
		loadImage(true);
	}
}


// IO
bool ImageProvider::isImg(QString s) {
	return s.endsWith(".png") || s.endsWith(".jpg") || s.endsWith(".jpeg") || s.endsWith(".gif");
}

void ImageProvider::loadDir() {
	files->clear();

	QString c = io->getCurrentDir();
	c = c.replace("\\", "/"); //  \/ Dir-only
	io->setCurrentDir(isImg(c) ? QStringRef(&c, 0, c.lastIndexOf('/')).toString() : (c.endsWith('/') ? c : c + '/'));

	for ( QFileInfo i : QDir(io->getCurrentDir()).entryInfoList({"*.png", "*.jpg", "*.jpeg", "*.gif"}, QDir::Files)) {
		files->append(i.absolutePath() + '/' + i.fileName());
	}

	if (isImg(c)) {
		c = c.remove(0, c.lastIndexOf('/') + 1); // File-name only
		for ( int i = 0; i < files->size(); i++ )
			if (files->value(i, "").endsWith(c))
				current = i;

	} else if(lastPicture.contains(getDir(io->getCurrentDir()))){
		current = lastPicture.value(getDir(io->getCurrentDir())).toInt(0);
		current = current < files->size() ? current : 0;
	} else {
		current = 0;
	}

	if (io->getCurrentDir() != "")
		loadImage();
}

void ImageProvider::del() {
	io->del(files->value(current));
	files->removeAt(current);
	loadImage();
}

void ImageProvider::cop() {
	io->cop(files->value(current));
	files->removeAt(current);
	loadImage();
}

void ImageProvider::exp() {
	io->exp(files->value(current));
	next(1);
}


// Setters
void ImageProvider::setText(QString t) {
	this->text = t;
	emit titleChanged(t);
}

void ImageProvider::setLoading(bool b) {
	isLoading = b;
	if(!isLoading)
		wnd->resetCoord(),
				wnd->update();
}

void ImageProvider::setOSCale(float f) {
	wnd->oscale = f;
}

void ImageProvider::setScale(float f) {
	wnd->scale = f;
}

void ImageProvider::setOriginal(QImage &i) {
	QMutexLocker m(mutex);
	this->iOriginal = i;
	wnd->update();
}

void ImageProvider::setScaled(QImage &i) {
	QMutexLocker m(mutex);
	this->iScaled = i;
	wnd->update();
}

void ImageProvider::loadNewDir(int inc) {
	if (isLoading)
		return;

	QString curr = io->getCurrentDir(), sDir = curr;
	curr = sDir.remove(sDir.length() - 1, 1); // Delete last slash
	sDir = sDir.remove(sDir.lastIndexOf('/'), 100); // Delete all before last slash

	QFileInfoList entr = QDir(sDir).entryInfoList({"*"}, QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
	int ind = 0;
	for (int i = 0; i < entr.size(); i++) {
		if (curr == entr[i].absoluteFilePath()) {
			ind = i;
		}
	}

	ind += inc;

	ind = ind < 0 ? 0 : ind;
	ind = ind > entr.length() - 1 ? entr.length() - 1 : ind;

	sDir = entr[ind].absoluteFilePath();

	io->setCurrentDir(sDir + '/');
	loadDir();

}



