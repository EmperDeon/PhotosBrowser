#include "pio.h"

PIO::PIO(ImageProvider *p): prv(p) {
	currentDir = "";
	deletedDir = sett.value("deletedDir", "").toString();
	savedDir = sett.value("savedDir", "").toString();
}

void PIO::setNewCurrentDir() {
	currentDir = QFileDialog::getExistingDirectory(this->prv->getWnd(), "Select dir with files", currentDir) + '/';
	prv->loadDir();
}

void PIO::setNewDeletedDir() {
	deletedDir = QFileDialog::getExistingDirectory(this->prv->getWnd(), "Select dir for deleted files", deletedDir) + '/';
	sett.setValue("deletedDir", deletedDir);
}

void PIO::setNewSavedDir() {
	savedDir = QFileDialog::getExistingDirectory(this->prv->getWnd(), "Select dir for saved files", savedDir) + '/';
	sett.setValue("savedDir", savedDir);
}

void PIO::openCurrentDir() {
	QDesktopServices::openUrl(QUrl::fromLocalFile(currentDir));
}

void PIO::del(QString file) {
	if (deletedDir == "")
		setNewDeletedDir();

	if (QFile(file).exists()) {
		file = file.remove(0, file.lastIndexOf('/') + 1);

		QFile::rename(currentDir + file, deletedDir + file);
	}
}

void PIO::cop(QString file) {
	if (savedDir == "")
		setNewSavedDir();

	if (QFile(file).exists()) {
		file = file.remove(0, file.lastIndexOf('/') + 1);

		QFile::rename(currentDir + file, savedDir + file);
	}
}

void PIO::exp(QString file) {
	if (savedDir == "")
		setNewSavedDir();

	file = file.remove(0, file.lastIndexOf('/') + 1);
	prv->getOriginal().save(savedDir + file);
}






