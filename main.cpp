#include "main.h"

#define log(s) QMessageBox::information(this, "", s)

int main(int argv, char** argc) {
	QApplication a(argv, argc);
	QCoreApplication::setOrganizationName("IlzSoft");
	QCoreApplication::setOrganizationDomain("github.com/ilz2010");
	QCoreApplication::setApplicationName("PhotosBrowser");

	QStringList l = a.arguments();
	PsWnd w(l.size() == 2 ? l[1] : "");
	w.showMaximized();

	return a.exec();
}

PsWnd::PsWnd(QString c) {
	files = new QStringList;
	delHs = new QStringList;

	dirCurrent = "";
	dirDeleted = sett.value("deletedDir", "").toString();
	dirGodnota = sett.value("godnotaDir", "").toString();

 m = new QMenu;
	m->addAction("Change \"Current\" dir", this, SLOT(setCDir()));
	m->addAction("Change \"Deleted\" dir", this, SLOT(setDDir()));
	m->addAction("Change \"GoodToSave\" dir", this, SLOT(setGDir()));
	m->addAction("Open current dir", this, SLOT(openDir()));
	m->addAction("Update image", this, SLOT(update()));

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showMenu(const QPoint&)));

	if(c==""){
		setCDir();
	}else{
		dirCurrent = c;
		load();
	}
}

// Slots
void PsWnd::setCDir(){
	dirCurrent = QFileDialog::getExistingDirectory(this, "Select dir with files", dirCurrent) + '/';
	load();
}

void PsWnd::setDDir(){
	dirDeleted = QFileDialog::getExistingDirectory(this, "Select dir for deleted files", dirDeleted) + '/';
	sett.setValue("deletedDir", dirDeleted);
}

void PsWnd::setGDir(){
	dirGodnota = QFileDialog::getExistingDirectory(this, "Select dir for saved files", dirGodnota) + '/';
	sett.setValue("godnotaDir", dirGodnota);
}

void PsWnd::openDir(){
	QDesktopServices::openUrl(QUrl::fromLocalFile(dirCurrent));
}

void PsWnd::next() {	current = current < lSize-1 ? current+1 : 0;	loadImage();}

void PsWnd::prev() {	current = current > 0 ? current-1 : lSize-1;	loadImage();}

void PsWnd::next(int i){
	current = current < lSize-i ? current + i : 0;	loadImage();
}

void PsWnd::prev(int i){
	current = current > i ? current-i : lSize-i;	loadImage();
}

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
bool PsWnd::isImg(QString s){
	return s.endsWith(".png") || s.endsWith(".jpg");
}

void PsWnd::load() {
	files->clear();

	QString c = dirCurrent;
	c = c.replace("\\", "/");
	dirCurrent = isImg(c) ? QStringRef(&c, 0, c.lastIndexOf('/')).toString() : (c.endsWith('/') ? c : c + '/');

	if(isImg(c)) c = c.remove(0, c.lastIndexOf('/')+1);

	for(QFileInfo i : QDir(dirCurrent).entryInfoList({"*.png", "*.jpg", "*.gif"}, QDir::NoDotAndDotDot | QDir::Files)){
		files->append(i.absolutePath() + '/' + i.fileName());
	}

	if(isImg(c)){
		for(int i = 0 ; i < files->size() ; i++)
			if(files->value(i, "").endsWith(c))
				current = i;
	}else{
		current = 0;
	}

	lSize = files->size();
	if(dirCurrent != "") loadImage();
}

void PsWnd::loadImage() {
	QString file = files->value(current);

	QPixmap in(file);
	int mw, mh;
	if(in.width() > in.height()){
		mh = in.height() > height() ? in.height() : height();
		mw = (float(width())/height() * (mh));
	}else{
		mw = in.width() > width() ? in.width() : width();
		mh = (float(width())/height() * (mw));
	}

	QPixmap p = r ? process(in, mw + 20, mh + 20) : in;
	this->image = p;
	back = getBackColor(p.toImage());

	simg = 	p.height() > height() || p.width() > width() ?
		p.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation) :	p;

	float s1 = float(height())/p.height(), s2 = float(width())/p.width();
	scale = qMin(s1, s2);
	scale = scale > 1.0f ? 1.0f : scale;

	oscale = scale;

	QString rat, size="%1 ";

	if(in.width() / 16 * 9 == in.height()){
		rat = "16/9";
	}else if(in.width() / 16 * 10 == in.height()){
		rat = "16/10";
	}else{
		rat = QString::number(float(in.width())/in.height(), 'f', 3);
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

	title = QString("File: %1 | Ratio: %2 (%3:%4) | Size: %5 | %6/%7 ");
	title = title.arg(file.size() < 100 ? file : QStringRef(&file, 0, 100).toString())
		.arg(rat)
		.arg(p.width())
		.arg(p.height())
		.arg(size.arg(s, 4, 'f', 3, ' '))
		.arg(current+1)
		.arg(this->lSize);
	this->setWindowTitle(title);

	tx = 0;
	ty = 0;
	//scale = 1.0f;
	update();
}

void PsWnd::chFull(){
	this->hide();
	if(f){
		this->setCursor(QCursor(Qt::ArrowCursor));
		this->showMaximized();
		f = false;
	}else{
		this->setCursor(QCursor(QPixmap(1,1)));
		this->showFullScreen();
		f = true;
	}
}

void PsWnd::chBlur(){
	r = !r;
	loadImage();
}
// Image loading


// Qt overrides
void PsWnd::keyPressEvent(QKeyEvent *event) {
	switch(event->key()){
	case 16777234: prev(); break;
	case 16777236: next(); break;
	case 16777237: prev(100); break;
	case 16777235: next(100); break;
	case 16777223: del();  break;
	case 32      : cop();  break;
	case 16777274:
	case 16777216: chFull(); break;
	case 66      : chBlur(); break;
	case 71      :
	case 1055    : current = QInputDialog::getInt(this, "Go to", "Go to:", current, 0, lSize)-1, loadImage();
	case 16777217: t = !t;   break;
	case Qt::Key_Z: und(); break;
	default: qDebug() << event->key();
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
	p.setBrush(QBrush(QColor(56, 56, 56, 255)));
	p.drawRect(-1, -1, width()+1, height()+1);

	x2 = int(image.width()  * scale) + 1;
	y2 = int(image.height() * scale) + 1;

	x1 = (width() - x2) / 2;
	y1 = (height() - y2) / 2;

	p.translate(tx, ty);
	p.drawPixmap(x1, y1, x2, y2, scale == oscale ? simg : image);

	if(f && t){
		p.setFont(QFont("Fantasque Sans Mono", 11));
		p.setBrush(QBrush(QColor(0, 0, 0, 127)));
		p.setPen(Qt::NoPen);

		QFontMetrics m(QFont("Fantasque Sans Mono", 11));
		int x = m.width(title) + 10, y = m.height()/2 + 17;

		p.drawRect((width() - x)/2, 0, x, y);

		p.setPen(QPen(QBrush(QColor(255,255,255)), 100));
		p.drawText((width() - x)/2, 0, x, y,  Qt::AlignCenter, title);
	}

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


// Process
#define BLUR_COUNT 10

QColor PsWnd::getBackColor(QImage c){
	quint64 r=0,g=0,b=0;
	int cn = c.width() + c.width() + c.height() + c.height() - 4;

	for(int i = 0 ; i < c.width() ; i++){
		QColor t(c.pixel(i, 0));
		r += t.red();
		g += t.green();
		b += t.blue();
		t = QColor(c.pixel(i, c.height()-1));
		r += t.red();
		g += t.green();
		b += t.blue();
	}
	for(int i = 1 ; i < c.height()-1 ; i++){
		QColor t(c.pixel(0, i));
		r += t.red();
		g += t.green();
		b += t.blue();
		t = QColor(c.pixel(c.width()-1, i));
		r += t.red();
		g += t.green();
		b += t.blue();
	}
	return QColor(r/cn, g/cn, b/cn);
}

QColor PsWnd::getWColor(QImage o, int i, int i1){
	int r = 0, g = 0, b = 0;
	int st, en;

	st = (i1==0 ? 0 : o.height() - BLUR_COUNT);
	en = (i1==0 ? BLUR_COUNT : o.height());

	for(int t = st ; t < en ; t++){
		QColor c = o.pixel(i, t);
		r += c.red();
		g += c.green();
		b += c.blue();
	}

	return QColor(r/BLUR_COUNT, g/BLUR_COUNT, b/BLUR_COUNT);
}

QColor PsWnd::getHColor(QImage o, int i, int i1){
	int r = 0, g = 0, b = 0;
	int st, en;

	st = (i1==0 ? 0 : o.width() - BLUR_COUNT);
	en = (i1==0 ? BLUR_COUNT : o.width());

	for(int t = st ; t < en ; t++){
		QColor c = o.pixel(t, i);
		r += c.red();
		g += c.green();
		b += c.blue();
	}

	return QColor(r/BLUR_COUNT, g/BLUR_COUNT, b/BLUR_COUNT);

}

QImage PsWnd::applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent){
	if(src.isNull()) return QImage();   //No need to do anything else!
	if(!effect) return src;             //No need to do anything else!
	QGraphicsScene scene;
	QGraphicsPixmapItem item;
	item.setPixmap(QPixmap::fromImage(src));
	item.setGraphicsEffect(effect);
	scene.addItem(&item);
	QImage res(src.size()+QSize(extent*2, extent*2), QImage::Format_ARGB32);
	res.fill(Qt::transparent);
	QPainter ptr(&res);
	scene.render(&ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent*2, src.height()+extent*2 ) );
	return res;
}

QPixmap PsWnd::process(QPixmap in, int MW, int MH){
	if(in.isNull()) return in;
	QImage o = in.toImage();
	QPixmap n = QPixmap(MW, MH);
	QPainter out(&n);
	QLinearGradient gr;

	int topX = (MW-o.width())/2, topY = (MH - o.height())/2;

	QColor back = getBackColor(o);

	out.fillRect(QRect(0, 0, MW, MH), QBrush(back));
	out.setPen(Qt::NoPen);


	// Gradients Center
	for(int i = topX ; i < o.width()-topX ; i++){
		gr = QLinearGradient(topX+i, topY+10, topX+i, 0);
		gr.setColorAt(0, getWColor(o, i, 0));
		gr.setColorAt(1, back);
		out.setBrush(QBrush(gr));
		out.drawRect(topX+i, 0, 1, topY+10);

		gr = QLinearGradient(topX+i, topY+o.height()-10, topX+i, MH);
		gr.setColorAt(0, getWColor(o, i, 1));
		gr.setColorAt(1, back);
		out.setBrush(QBrush(gr));
		out.drawRect(topX+i, topY+o.height()-10, 1, topY);
	}

	for(int i = topY ; i < o.height()-topY ; i++){
		gr = QLinearGradient(topX+10, topY+i, 0, topY+i);
		gr.setColorAt(0, getHColor(o, i, 0));
		gr.setColorAt(1, back);
		out.setBrush(QBrush(gr));
		out.drawRect(0, topY+i, topX+10, 1);

		gr = QLinearGradient(topX+o.width()-10, topY+i, MW, topY+i);
		gr.setColorAt(0, getHColor(o, i, 1));
		gr.setColorAt(1, back);
		out.setBrush(QBrush(gr));
		out.drawRect(topX+o.width()-10, topY+i, topX, 1);
	}
	// Gradient center


	// Gradient angles
	for(int i = 0 ; i < topX ; i++){
		gr = QLinearGradient(topX+i, topY+10, i*2, 0);
		gr.setColorAt(0, getWColor(o, i, 0));
		gr.setColorAt(1, back);
		out.setPen(QPen(QBrush(gr), 2));
		out.drawLine(i*2, 0, topX+i, topY+10);

		gr = QLinearGradient(o.width() - topX + i, topY+10, i*2, 0);
		gr.setColorAt(0, getWColor(o, o.width() - topX + i, 0));
		gr.setColorAt(1, back);
		out.setPen(QPen(QBrush(gr), 1));
		out.drawLine(o.width()+ i*2, 0, o.width() + i, topY+10);

		gr = QLinearGradient(topX+i, topY+o.height()-10, i*2, MH);
		gr.setColorAt(0, getWColor(o, i, 1));
		gr.setColorAt(1, back);
		out.setPen(QPen(QBrush(gr), 2));
		out.drawLine(i*2, MH-1, topX+i, topY + o.height() -10);

		gr = QLinearGradient(o.width() - topX+i, topY+o.height()-10, i*2, MH);
		gr.setColorAt(0, getWColor(o, o.width() - topX + i, 1));
		gr.setColorAt(1, back);
		out.setPen(QPen(QBrush(gr), 2));
		out.drawLine(o.width() + i*2, MH-1, o.width()+i, topY + o.height() -10);
	}

	for(int i = 0 ; i < topY ; i++){
		gr = QLinearGradient(topX+10, topY+i, 0, i*2);
		gr.setColorAt(0, getHColor(o, i, 0));
		gr.setColorAt(1, back);
		out.setPen(QPen(QBrush(gr), 2));
		out.drawLine(0, i*2, topX+10, topY+i);

		gr = QLinearGradient(topY+10,o.height() - topY + i, 0,  i*2);
		gr.setColorAt(0, getHColor(o, o.height() - topY + i, 0));
		gr.setColorAt(1, back);
		out.setPen(QPen(QBrush(gr), 1));
		out.drawLine(0, o.height()+ i*2, topX+10, o.height() + i);

		gr = QLinearGradient(topX+o.width()-10, topY+i, MH, i*2);
		gr.setColorAt(0, getHColor(o, i, 1));
		gr.setColorAt(1, back);
		out.setPen(QPen(QBrush(gr), 2));
		out.drawLine(MW-1, i*2, topX + o.width() - 10, topY+i);

		gr = QLinearGradient(topX+o.width()-10, o.height() - topY+i, MH, i*2);
		gr.setColorAt(0, getHColor(o, o.height() - topY + i, 1));
		gr.setColorAt(1, back);
		out.setPen(QPen(QBrush(gr), 2));
		out.drawLine(MW-1, o.height() + i*2, topX + o.width() -10, o.height()+i);
	}
	// Gradients


	// Blur
	QGraphicsBlurEffect *eb = new QGraphicsBlurEffect;
	eb->setBlurRadius(20);
	eb->setBlurHints(QGraphicsBlurEffect::QualityHint);
	out.end();

	n = QPixmap::fromImage(applyEffectToImage(n.toImage(), eb));

	out.begin(&n);
	// Blur


	// Shadow
	out.setBrush(QBrush(QColor(0, 0, 0, 15)));
	out.drawRect(0,0,MW, MH);

	QGraphicsDropShadowEffect *e = new QGraphicsDropShadowEffect;
	e->setBlurRadius(100);
	e->setColor(Qt::black);
	o = applyEffectToImage(o, e, 150);
	// Shadow


	// Original
	topX = (MW-o.width())/2;
	topY = (MH - o.height())/2;

	out.drawImage(topX, topY, o);
	// Original
	out.end();

	return n.copy(10, 10, MW-20, MH-20);
}
// Process
