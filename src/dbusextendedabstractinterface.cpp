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


#include <DBusExtendedAbstractInterface>

#include <QtDBus/QDBusMetaType>

#include <QtCore/QDebug>
#include <QtCore/QMetaProperty>


Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, dBusPropertiesInterface, ("org.freedesktop.DBus.Properties"))
Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, dBusPropertiesChangedSignal, ("PropertiesChanged"))
Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, propertyChangedSignature, ("propertyChanged(QString,QVariant)"))
Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, propertyInvalidatedSignature, ("propertyInvalidated(QString)"))


DBusExtendedAbstractInterface::DBusExtendedAbstractInterface(const QString &service, const QString &path, const char *interface, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, interface, connection, parent)
    , m_lastPropertyChangedError()
    , m_propertiesChangedConnected(false)
{
}

DBusExtendedAbstractInterface::~DBusExtendedAbstractInterface()
{
}

QDBusError DBusExtendedAbstractInterface::lastPropertyChangedError() const
{
    return m_lastPropertyChangedError;
}

void DBusExtendedAbstractInterface::connectNotify(const QMetaMethod &signal)
{
    if (signal.methodType() == QMetaMethod::Signal
        && (signal.methodSignature() == *propertyChangedSignature()
            || signal.methodSignature() == *propertyInvalidatedSignature())) {
        if (!m_propertiesChangedConnected) {
            QStringList argumentMatch;
            argumentMatch << interface();
            connection().connect(service(), path(), *dBusPropertiesInterface(), *dBusPropertiesChangedSignal(),
                                 argumentMatch, QString(),
                                 this, SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));

            m_propertiesChangedConnected = true;
            return;
        }
    } else {
        QDBusAbstractInterface::connectNotify(signal);
    }
}

void DBusExtendedAbstractInterface::disconnectNotify(const QMetaMethod &signal)
{
    if (signal.methodType() == QMetaMethod::Signal
        && (signal.methodSignature() == *propertyChangedSignature()
            || signal.methodSignature() == *propertyInvalidatedSignature())) {
        if (m_propertiesChangedConnected
            && 0 == receivers(propertyChangedSignature()->constData())
            && 0 == receivers(propertyInvalidatedSignature()->constData())) {
            QStringList argumentMatch;
            argumentMatch << interface();
            connection().disconnect(service(), path(), *dBusPropertiesInterface(), *dBusPropertiesChangedSignal(),
                                 argumentMatch, QString(),
                                 this, SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));

            m_propertiesChangedConnected = false;
            return;
        }
    } else {
        QDBusAbstractInterface::disconnectNotify(signal);
    }
}

void DBusExtendedAbstractInterface::onPropertiesChanged(const QString& interfaceName,
                                                        const QVariantMap& changedProperties,
                                                        const QStringList& invalidatedProperties)
{
    if (interfaceName == interface()) {
        QVariantMap::const_iterator i = changedProperties.constBegin();
        while (i != changedProperties.constEnd()) {
            int propertyIndex = metaObject()->indexOfProperty(i.key().toLatin1().constData());

            if (-1 == propertyIndex) {
                qDebug() << Q_FUNC_INFO << "Got unknown changed property" <<  i.key();
            } else {
                QVariant value = demarshall(interface(), metaObject()->property(propertyIndex), i.value(), &m_lastPropertyChangedError);

                if (m_lastPropertyChangedError.isValid()) {
                    emit propertyInvalidated(i.key());
                } else {
                    emit propertyChanged(i.key(), value);
                }
            }

            ++i;
        }

        QStringList::const_iterator j = invalidatedProperties.constBegin();
        while (j != invalidatedProperties.constEnd()) {
            if (-1 == metaObject()->indexOfProperty(j->toLatin1().constData())) {
                qDebug() << Q_FUNC_INFO << "Got unknown invalidated property" <<  *j;
            } else {
                m_lastPropertyChangedError = QDBusError();
                emit propertyInvalidated(*j);
            }

            ++j;
        }
    }
}

QVariant DBusExtendedAbstractInterface::demarshall(const QString &interface, const QMetaProperty &metaProperty, const QVariant &value, QDBusError *error)
{
    Q_ASSERT(metaProperty.isValid());
    Q_ASSERT(error != 0);

    if (value.userType() == metaProperty.userType()) {
        // No need demarshalling. Passing back straight away ...
        *error = QDBusError();
        return value;
    }

    QVariant result = QVariant(metaProperty.userType(), (void*)0);
    QString errorMessage;
    const char *expectedSignature = QDBusMetaType::typeToSignature(metaProperty.userType());

    if (value.userType() == qMetaTypeId<QDBusArgument>()) {
        // demarshalling a DBus argument ...
        QDBusArgument dbusArg = value.value<QDBusArgument>();

        if (expectedSignature == dbusArg.currentSignature().toLatin1()) {
            QDBusMetaType::demarshall(dbusArg, metaProperty.userType(), result.data());
            if (!result.isValid()) {
                errorMessage = QStringLiteral("Unexpected failure demarshalling "
                                              "upon PropertiesChanged signal arrival "
                                              "for property `%3.%4' (expected type `%5' (%6))")
                    .arg(interface,
                         QString::fromLatin1(metaProperty.name()),
                         QString::fromLatin1(metaProperty.typeName()),
                         expectedSignature);
            }
        } else {
                errorMessage = QStringLiteral("Unexpected `user type' (%2) "
                                              "upon PropertiesChanged signal arrival "
                                              "for property `%3.%4' (expected type `%5' (%6))")
                    .arg(dbusArg.currentSignature(),
                         interface,
                         QString::fromLatin1(metaProperty.name()),
                         QString::fromLatin1(metaProperty.typeName()),
                         QString::fromLatin1(expectedSignature));
        }
    } else {
        const char *actualSignature = QDBusMetaType::typeToSignature(value.userType());

        errorMessage = QStringLiteral("Unexpected `%1' (%2) "
                                      "upon PropertiesChanged signal arrival "
                                      "for property `%3.%4' (expected type `%5' (%6))")
            .arg(QString::fromLatin1(value.typeName()),
                 QString::fromLatin1(actualSignature),
                 interface,
                 QString::fromLatin1(metaProperty.name()),
                 QString::fromLatin1(metaProperty.typeName()),
                 QString::fromLatin1(expectedSignature));
    }

    if (errorMessage.isEmpty()) {
        *error = QDBusError();
    } else {
        *error = QDBusMessage::createError(QDBusError::InvalidSignature, errorMessage);
        qDebug() << Q_FUNC_INFO << errorMessage;
    }

    return result;
}

