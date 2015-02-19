// -*- c++ -*-

/*!
 *
 * Copyright (C) 2015 Jolla Ltd.
 *
 * Contact: Valerio Valerio <valerio.valerio@jolla.com>
 * Author: Andres Gomez <andres.gomez@jolla.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef DBUSEXTENDEDABSTRACTINTERFACE_H
#define DBUSEXTENDEDABSTRACTINTERFACE_H

#include <DBusExtended>

#include <QDBusAbstractInterface>
#include <QDBusError>

class QT_DBUS_EXTENDED_EXPORT DBusExtendedAbstractInterface: public QDBusAbstractInterface
{
    Q_OBJECT

public:
    virtual ~DBusExtendedAbstractInterface();

    QDBusError lastPropertyChangedError() const;

    // It could be interesting to have a "refresh" method implementing
    // the "GetAll" DBus properties method in order to improve DBUs
    // traffic performance since we already have a demarshalling
    // implementation but we don't really need it by now. In addition,
    // it is unclear how to face the value change in properties
    // without notification signal.

protected:
    DBusExtendedAbstractInterface(const QString &service,
                                  const QString &path,
                                  const char *interface,
                                  const QDBusConnection &connection,
                                  QObject *parent);

    void connectNotify(const QMetaMethod &signal);
    void disconnectNotify(const QMetaMethod &signal);

Q_SIGNALS:
    void propertyChanged(const QString &propertyName, const QVariant &value);
    void propertyInvalidated(const QString &propertyName);

private Q_SLOTS:
    void onPropertiesChanged(const QString& interfaceName,
                             const QVariantMap& changedProperties,
                             const QStringList& invalidatedProperties);
private:
    static QVariant demarshall(const QString &interface, const QMetaProperty &metaProperty, const QVariant &value, QDBusError *error);

    QDBusError m_lastPropertyChangedError;
    bool m_propertiesChangedConnected;
};

#endif /* DBUSEXTENDEDABSTRACTINTERFACE_H */
