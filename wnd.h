#ifndef PHOTOSBROWSER_WND_H
#define PHOTOSBROWSER_WND_H
#include <QtWidgets>

class PsWnd : public QMainWindow{
	Q_OBJECT

 QString dirDeleted;
	QString dirCurrent;

	QStringList* files;

	int current;
public slots:
	void setCDir();
 void setDDir();

public:
	PsWnd();

protected:
	virtual void keyPressEvent(QKeyEvent *keyEvent) override;
	void load();
};

#endif //PHOTOSBROWSER_WND_H
