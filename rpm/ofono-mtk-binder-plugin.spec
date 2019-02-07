Name: ofono-mtk-binder-plugin
Version: 0.0.1
Release: 1
Summary: Radio interfaces for Mtk
Group: Development/Libraries
License: BSD
URL: https://github.com/neochapay/ofono-mtk-binder-plugin
Source: %{name}-%{version}.tar.bz2

Requires: ofono >= 1.21+git38
Requires: libgbinder >= 1.0.26
Requires: libgbinder-radio >= 1.0.0
BuildRequires: ofono-devel >= 1.21+git38
BuildRequires: pkgconfig(libgbinder) >= 1.0.26
BuildRequires: pkgconfig(libgbinder-radio) >= 1.0.0

%description
Implements radio binder interfaces for MTK devices

%prep
%setup -q -n %{name}-%{version}

%build
./autogen.sh
%configure
make %{_smp_mflags}

%install
rm -rf %{buildroot}
%make_install
#libtool don't like lib without version
rm %{buildroot}/usr/lib/ofono/plugins/mtkbinderplugin.so.0
rm %{buildroot}/usr/lib/ofono/plugins/mtkbinderplugin.so
mv %{buildroot}/usr/lib/ofono/plugins/mtkbinderplugin.so.0.0.0 %{buildroot}/usr/lib/ofono/plugins/mtkbinderplugin.so

%files
%defattr(-,root,root,-)
%{_libdir}/ofono/plugins/mtkbinderplugin.so
