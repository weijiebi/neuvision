//
// Z3D - A structured light 3D scanner
// Copyright (C) 2013-2016 Nicolas Ulrich <nikolaseu@gmail.com>
//
// This file is part of Z3D.
//
// Z3D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Z3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Z3D.  If not, see <http://www.gnu.org/licenses/>.
//

#include "ZGui/zapplication.h"

#include "ZCore/zlogging.h"
#include "ZCore/zloghandler_p.h"
#include "ZCore/zpluginloader.h"

#include <QFileInfo>
#include <QSplashScreen>
#include <QThreadPool>

Z3D_LOGGING_CATEGORY_FROM_FILE("z3d.zgui", QtInfoMsg)

namespace Z3D
{

ZApplication::ZApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_splash(nullptr)
{
    /// set message handler for debug
#if !defined(Q_OS_ANDROID)
    qInstallMessageHandler(&ZLogHandler);
#endif

#ifdef Z3D_THREAD_COUNT_LIMIT
    QThreadPool::globalInstance()->setMaxThreadCount(Z3D_THREAD_COUNT_LIMIT);
#endif

    setOrganizationDomain("z3d.neuvision");
    setOrganizationName("Z3D");
    setApplicationName(QFileInfo(argv[0]).baseName());
    setApplicationVersion(Z3D_VERSION_BUILD_STR);

    zInfo() << "starting" << applicationName()
            << "version" << applicationVersion();

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL, true);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
}

void ZApplication::loadPlugins()
{
    ZPluginLoader::instance()->loadPlugins();
}

QSplashScreen * ZApplication::showSplashScreen()
{
    if (!m_splash) {
        QPixmap pixmap(":/splash");
        m_splash = new QSplashScreen(pixmap);
        m_splash->show();

        connect(ZPluginLoader::instance(), &ZPluginLoader::progressChanged,
                [&](float progress, const QString &message) {
                    zDebug() << "progress:" << progress << "message:" << message;
                    m_splash->showMessage(message);
                    processEvents();
                });
    }

    return m_splash;
}

void ZApplication::finishSplashScreen(QWidget *widget)
{
    if (m_splash) {
        m_splash->finish(widget);
    }
}

} // namespace Z3D
