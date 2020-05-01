Name:       dbusextended-qt5

Summary:    Extended DBus for Qt
Version:    0.0.2
Release:    1
Group:      Development/Libraries
License:    LGPLv2.1
URL:        https://github.com/nemomobile/qtdbusextended
Source0:    %{name}-%{version}.tar.bz2
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)

%description
%{summary}.

%package devel
Summary:    Development files for %{name}
Requires:   %{name} = %{version}-%{release}

%description devel
Development files for %{name}.

%prep
%setup -q -n %{name}-%{version}

%build
mkdir build
cd build
cmake \
	-DCMAKE_BUILD_TYPE=None \
	-DCMAKE_INSTALL_PREFIX=%{_prefix} \
	-DCMAKE_INSTALL_LIBDIR=%{_lib} \
	-DCMAKE_VERBOSE_MAKEFILE=ON \
	..
cmake --build .

%install
cd build
rm -rf %{buildroot}
DESTDIR=%{buildroot} cmake --build . --target install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/lib*.so.*

%files devel
%defattr(-,root,root,-)
%{_datarootdir}/qt5/mkspecs/features/%{name}.prf
%{_includedir}/dbusextended/DBusExtended
%{_includedir}/dbusextended/DBusExtendedAbstractInterface
%{_includedir}/dbusextended/dbusextended.h
%{_includedir}/dbusextended/dbusextendedabstractinterface.h
%{_libdir}/lib*.so
%{_libdir}/pkgconfig/%{name}.pc
%{_libdir}/cmake/dbusextended/*.cmake
