#ifndef PHOTOSBROWSER_PIO_H
#define PHOTOSBROWSER_PIO_H
#include <imageProvider.h>

class ImageProvider;

class PIO : public QObject{
	Q_OBJECT

 ImageProvider *prv;

	QSettings sett;
	QString currentDir, deletedDir, savedDir;
	QStringList* delHs;

public slots:
	void setNewCurrentDir();
	void setNewDeletedDir();
	void setNewSavedDir();
 void openCurrentDir();

public:
	PIO(ImageProvider *p);

 void del(QString file);
	void cop(QString file);

	const QString &getCurrentDir() const { return currentDir; }
	const QString &getDeletedDir() const { return deletedDir; }
	const QString &getSavedDir() const { return savedDir; }


	void setCurrentDir(const QString &currentDir) { this->currentDir = currentDir; }
	void setDeletedDir(const QString &deletedDir) { this->deletedDir = deletedDir; }
	void setSavedDir(const QString &savedDir) { this->savedDir = savedDir; }

	void exp(QString file);
};

#endif //PHOTOSBROWSER_PIO_H
