#ifndef PHOTOSBROWSER_MAIN_H
#define PHOTOSBROWSER_MAIN_H
#include <QtWidgets>
#include <resizer.h>

class Resize;

class PsWnd : public QMainWindow{
Q_OBJECT

	Resize* resize;

	QFont font_fan;
	QMenu* m;
	QImage image, simg;
	int tx, ty, px, py;
	float scale, oscale;

	QSettings sett;
	QString dirDeleted, dirCurrent, dirSave, title;

	QStringList* files;
	QStringList* delHs;
	int lSize, current;

	bool f = false, t = false, r = true;

	// Gif
	QMovie* g_mov;
//	bool isGif;

	bool isImg(QString s);

	void load();
	void loadImage();

	void del();
	void cop();
	void und();
	void exportCurrent();

	QImage process(QImage in);

private slots:
	void nextFrame(int i);

	void showMenu(const QPoint& p){ m->popup(p);}
	void setCDir();
	void setDDir();
	void setGDir();
	void openDir();

	void next(int i);
	void prev(int i);
	void chFull();
	void chBlur();

public:
	PsWnd(QString c = "");

protected:
	virtual void keyPressEvent(QKeyEvent *keyEvent) override;
	virtual void wheelEvent(QWheelEvent *qWheelEvent) override;
	virtual void paintEvent(QPaintEvent *qPaintEvent) override;
	virtual void mousePressEvent(QMouseEvent *mouseEvent) override;
	virtual void mouseMoveEvent(QMouseEvent *mouseEvent) override;

};

#endif //PHOTOSBROWSER_MAIN_H
