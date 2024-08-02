#include "statusbar.h"

#include <obs-frontend-api.h>
#include <QGuiApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include <QDir>
#include <QFileSystemWatcher>
#include <QStringList>
#include <QSet>
#include <QFile>
#include <QTextStream>

#include <obs-module.h>
#include <obs.hpp>
#include <obs-config.h>
#include <plugin-support.h>

// Global variables to hold configuration values
static QWidget *container = nullptr;
static QHBoxLayout *layout = nullptr;
static QFileSystemWatcher *fileWatcher = nullptr;
static QMap<QString, QLabel *> labelsMap;

template<typename T> QSet<T> QListToQSet(const QList<T> &qlist)
{
	return QSet<T>(qlist.constBegin(), qlist.constEnd());
}

QString getStatusbarInfoPath()
{
	char *configPath = obs_module_config_path("");
	QDir dir(QString::fromUtf8(configPath));
	if (!dir.exists()) {
		dir.mkpath(".");
		obs_log(LOG_INFO, "Created directory: %s",
			dir.absolutePath().toUtf8().constData());
	} else {
		obs_log(LOG_INFO, "Using existing directory: %s",
			dir.absolutePath().toUtf8().constData());
	}
	bfree(configPath);
	return dir.absolutePath();
}

void updateLabels()
{
	obs_log(LOG_INFO, "Updating labels");

	// Get the status-bar-infos directory
	QString statusbarInfoPath = getStatusbarInfoPath();

	// List all text files in the directory
	QDir dir(statusbarInfoPath);
	QStringList filters;
	filters << "*.txt";
	QStringList textFiles = dir.entryList(filters, QDir::Files);

	obs_log(LOG_INFO, "Found %d text files", textFiles.size());

	// Convert lists to sets for easier comparison
	QSet<QString> currentFiles = QListToQSet(labelsMap.keys());
	QSet<QString> newFiles = QListToQSet(textFiles);

	// Remove labels for deleted files
	for (const QString &file : currentFiles - newFiles) {
		obs_log(LOG_INFO, "Removing label for deleted file: %s",
			file.toUtf8().constData());
		delete labelsMap[file];
		labelsMap.remove(file);
	}

	// Add labels for new files and update existing ones
	for (const QString &file : newFiles) {
		QString filePath = statusbarInfoPath + "/" + file;
		QFile textFile(filePath);

		if (textFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream in(&textFile);
			QString fileContent = in.readAll()
						      .replace("\r\n", "")
						      .replace("\n", "");
			textFile.close();

			if (labelsMap.contains(file)) {
				obs_log(LOG_INFO, "Updating label for file: %s",
					file.toUtf8().constData());
				labelsMap[file]->setText(fileContent);
			} else {
				obs_log(LOG_INFO,
					"Adding label for new file: %s",
					file.toUtf8().constData());
				QLabel *label =
					new QLabel(fileContent, container);
				labelsMap[file] = label;
				layout->addWidget(label);
			}
		} else {
			obs_log(LOG_ERROR, "Error reading file: %s",
				filePath.toUtf8().constData());
			if (labelsMap.contains(file)) {
				labelsMap[file]->setText("Error reading file");
			} else {
				QLabel *label = new QLabel("Error reading file",
							   container);
				labelsMap[file] = label;
				layout->addWidget(label);
			}
		}
	}
}

bool statusbar_init()
{
	blog(LOG_INFO, "Initializing status bar");

	auto mainWindow = (QMainWindow *)obs_frontend_get_main_window();

	// Set up status bar
	container = new QWidget();
	layout = new QHBoxLayout();

	layout->setContentsMargins(5, 0, 5, 0);
	container->setLayout(layout);
	mainWindow->statusBar()->addPermanentWidget(container);

	// Set up file system watcher
	fileWatcher = new QFileSystemWatcher(container);
	QString statusbarInfoPath = getStatusbarInfoPath();
	fileWatcher->addPath(statusbarInfoPath);

	QObject::connect(fileWatcher, &QFileSystemWatcher::directoryChanged,
			 updateLabels);

	// Initial population of labels
	updateLabels();

	return true;
}
