#ifndef PHOTOSBROWSER_MAIN_H
#define PHOTOSBROWSER_MAIN_H
#include <QtWidgets>
#include <imageProvider.h>

class ImageProvider;

class PsWnd : public QMainWindow{
 Q_OBJECT

 ImageProvider *provider;

	QTimer *loadingTimer;
	quint32 loadingAngle;

	QMenu* m;
	int tx, ty, px, py;
	float scale, oscale, tscale = 0.05f;

	bool isFullScreen = false, isText = false;

private slots:
	void showMenu(const QPoint& p){ m->popup(p);}
	void toggleFullScreen();

public:
	PsWnd(QString dir = "");

	void resetCoord();

protected:
	virtual void keyPressEvent(QKeyEvent *keyEvent) override;
	virtual void wheelEvent(QWheelEvent *qWheelEvent) override;
	virtual void paintEvent(QPaintEvent *qPaintEvent) override;
	virtual void mouseMoveEvent(QMouseEvent *mouseEvent) override;
	virtual void mousePressEvent(QMouseEvent *mouseEvent) override;
	virtual void closeEvent(QCloseEvent *event);

	friend class ImageProvider;
};

#endif //PHOTOSBROWSER_MAIN_H
