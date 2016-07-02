#include "imageProvider.h"
#include <QtConcurrent/QtConcurrent>

ImageProvider::ImageProvider(PsWnd *w, QString dir) : wnd(w) {
 io = new PIO(this);
 files = new QStringList;
	imgLoader = new ImageLoader(this);
	gifLoader = new GifLoader(this);
 mutex = new QMutex;

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
		case 16777237: prev(100); break;
		case 16777235: next(100); break;
		case 16777223: del(); break;
		case 32      : cop(); break;
		case 69      : exp(); break;
		case 66      : toggleBlur(); break;
		case 71      :
		case 1055    : goTo(); break;
		default      : qDebug() << i; break;
	}
}

void ImageProvider::closeEvent() {
	if(current != 0 && files->size() != 0){
		lastPicture.insert(getDir(files->value(0)), current);
	}

 QFile f(QApplication::applicationDirPath() + "/photos_last.json");
	if(f.open(QFile::WriteOnly)){
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
		loadImage();
	}
}

void ImageProvider::loadImage() {
	QString file = files->value(current);

 gifLoader->stop();
	if (file.endsWith(".gif")) {
	 gifLoader->loadImage(file);

	} else {
	 if(!isLoading)
		QtConcurrent::run(imgLoader, &ImageLoader::loadImage, file);
	}
}

void ImageProvider::goTo() {
	current = QInputDialog::getInt(wnd, "Go to", "Go to:", current, 0, files->size()) - 1;
	loadImage();
}

void ImageProvider::toggleBlur() {
	if(loaderType == 0){
		imgLoader->toggleBlur();
		loadImage();
	}
}


// IO
bool ImageProvider::isImg(QString s) {
	return s.endsWith(".png") || s.endsWith(".jpg") || s.endsWith(".jpeg") || s.endsWith(".gif");
}

void ImageProvider::loadDir() {
	if(current != 0 && files->size() != 0){
  lastPicture.insert(getDir(files->value(0)), current);
	}
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

	}else{
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
	wnd->setWindowTitle(t);
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



