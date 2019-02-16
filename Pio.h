/*
	Copyright (c) 2017-2019 by Ilya Barykin
	Released under the MIT License.
	See the provided LICENSE.TXT file for details.
*/

#ifndef PHOTOSBROWSER_PIO_H
#define PHOTOSBROWSER_PIO_H

#include <ImageProvider.h>
#include <QtCore/QObject>
#include <QtCore/QSettings>


class ImageProvider;

class PIO : public QObject {
    ImageProvider *prv;

    QSettings sett;
    QString currentDir, deletedDir, savedDir;
    QStringList *delHs;

public:
    void setNewCurrentDir();

    void setNewDeletedDir();

    void setNewSavedDir();

    void openCurrentDir();

public:
    explicit PIO(ImageProvider *p);

    void del(QString file);

    void cop(QString file);

    const QString &getCurrentDir() const { return currentDir; }

    const QString &getDeletedDir() const { return deletedDir; }

    const QString &getSavedDir() const { return savedDir; }

    QString getCurrentName() const;


    void setCurrentDir(const QString &currentDir) { this->currentDir = currentDir; }

    void setDeletedDir(const QString &deletedDir) { this->deletedDir = deletedDir; }

    void setSavedDir(const QString &savedDir) { this->savedDir = savedDir; }

    void exp(QString file);
};

#endif //PHOTOSBROWSER_PIO_H
