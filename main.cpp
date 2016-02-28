#include "main.h"

int main(int argv, char** argc) {
	QApplication a(argv, argc);
	QString s = "^Mess^abcd^asd^";
	qDebug() << s.split("^");
//	PsWnd w;
//	w.showMaximized();

	return a.exec();
}

PsWnd::PsWnd() {
	files = new QStringList;
	delHs = new QStringList;
	dirCurrent = "";
	dirDeleted = "";
	dirGodnota = "";

 m = new QMenu;
	m->addAction("Change \"Current\" dir", this, SLOT(setCDir()));
	m->addAction("Change \"Deleted\" dir", this, SLOT(setDDir()));
	m->addAction("Change \"GoodToSave\" dir", this, SLOT(setGDir()));
	m->addAction("Open current dir", this, SLOT(openDir()));
	m->addAction("Update image", this, SLOT(update()));

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showMenu(const QPoint&)));

	setCDir();
}

// Slots
void PsWnd::setCDir(){	dirCurrent = QFileDialog::getExistingDirectory(this, "Select dir with files", dirCurrent) + '/';	load();}

void PsWnd::setDDir(){	dirDeleted = QFileDialog::getExistingDirectory(this, "Select dir for deleted files", dirDeleted) + '/';}

void PsWnd::setGDir(){ dirGodnota = QFileDialog::getExistingDirectory(this, "Select dir for saved files", dirGodnota) + '/';}

void PsWnd::openDir(){
	QDesktopServices::openUrl(QUrl::fromLocalFile(dirCurrent));
}

void PsWnd::next() {	current = current < lSize-1 ? current+1 : 0;	loadImage();}

void PsWnd::prev() {	current = current > 0 ? current-1 : lSize-1;	loadImage();}

void PsWnd::del() {
	if(dirDeleted == "") setDDir();
	QString file = files->value(current);
	if(QFile(file).exists()){
		file = file.remove(0, file.lastIndexOf('/')+1);
		if(delHs->size() > 10){
			delHs->removeFirst();
		}
		delHs->push_back(file);

		QFile::rename(dirCurrent + file, dirDeleted + file);
		files->removeAt(current);
	}
	current--;
	lSize--;

	next();
}

void PsWnd::cop(){
if(dirGodnota == "") setGDir();
QString file = files->value(current);
if(QFile(file).exists()){
	file = file.remove(0, file.lastIndexOf('/')+1);

	QFile::rename(dirCurrent + file, dirGodnota + file);
	files->removeAt(current);
}
current--;
lSize--;

next();
}

void PsWnd::und() {
	if(!delHs->isEmpty()){
		QString file = delHs->takeLast();
		QFile::rename(dirDeleted + file, dirCurrent + file);
		lSize++;
		current++;
	}
}
// Slots


// Image loading
void PsWnd::load() {
	files->clear();
	for(QFileInfo i : QDir(dirCurrent).entryInfoList({"*.png", "*.jpg", "*.gif"}, QDir::NoDotAndDotDot | QDir::Files)){
		files->append(i.absolutePath() + '/' + i.fileName());
	}

	lSize = files->size();
	current = 0;
	if(dirCurrent != "") loadImage();
}

void PsWnd::loadImage() {
	QString file = files->value(current);
	QPixmap p(file);
	this->image = p;

		simg = 	p.height() > height() || p.width() > width() ?
			p.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation) :	p;

	float s1 = float(height())/p.height(), s2 = float(width())/p.width();
	scale = qMin(s1, s2);
	scale = scale > 1.0f ? 1.0f : scale;

	oscale = scale;

	QString rat, size="%1 ";

	if(p.width() / 16 * 9 == p.height()){
		rat = "16/9";
	}else if(p.width() / 16 * 10 == p.height()){
		rat = "16/10";
	}else{
		rat = QString::number(float(p.width())/p.height(), 'f', 3);
	}

	double s = QFile(file).size();
	if(s > 1024*1024){
		s /= 1024*1024;
		size += " Mb";
	}else if(s > 1024){
		s /= 1024;
		size += "Kb";
	}else{
		size += "b";
	}

	file = file.remove(0, file.lastIndexOf('/')+1);

	QString string("File: %1 | Ratio: %2 (%3:%4) | Size: %5 | %6/%7 ");
	string = string.arg(file.size() < 100 ? file : QStringRef(&file, 0, 100).toString())
		.arg(rat)
		.arg(p.width())
		.arg(p.height())
		.arg(size.arg(s, 4, 'f', 3, ' '))
		.arg(current+1)
		.arg(this->lSize);
	this->setWindowTitle(string);

	tx = 0;
	ty = 0;
	//scale = 1.0f;
	update();
}
// Image loading


// Qt overrides
void PsWnd::keyPressEvent(QKeyEvent *event) {
	switch(event->key()){
	case 16777234: prev(); break;
	case 16777236: next(); break;
	case 16777223: del();  break;
	case 32      : cop();  break;
	case Qt::Key_Z: und(); break;
	default:;
	}
}

void PsWnd::wheelEvent(QWheelEvent *event) {
 if(event->angleDelta().y() > 0){
		scale += 0.5f;
	}else if(scale > oscale+0.01f){
		scale -= 0.5f;
 }else{
		scale = oscale;
	 tx = 0; ty = 0;
 }
	this->update();
}

void PsWnd::paintEvent(QPaintEvent *event) {
	static bool first = true;	if(first) loadImage(), first = false;
	QPainter p(this);

	int x1,y1,x2,y2;
	p.setBrush(QBrush(QColor(56, 56, 56)));
 p.drawRect(0, 0, width(), height());


	x2 = int(image.width()  * scale);
	y2 = int(image.height() * scale);

	x1 = (width() - x2) / 2;
	y1 = (height() - y2) / 2;

//	if(x2 > width() || y2 > height()){
//  int cx=0, cy=0, cw=image.width(), ch=image.height();

//		x1 = x1 > 0 ? x1 : 0;
//		y1 = y1 > 0 ? y1 : 0;



//  cx += tx/4;
//		cy += ty/4;

//		p.drawPixmap(x1, y1, x2, y2, image.copy(cx + scale*100, cy + scale*100, cw, ch));
//	}else{
		p.translate(tx, ty);
		p.drawPixmap(x1, y1, x2, y2, scale == oscale ? simg : image);
//	}



	p.end();
	event->accept();
}

void PsWnd::mouseMoveEvent(QMouseEvent *event) {
	tx += event->x() - px;
	ty += event->y() - py;
	px = event->x();
	py = event->y();
	update();
}

void PsWnd::mousePressEvent(QMouseEvent *event) {
	px = event->x();
	py = event->y();
}

