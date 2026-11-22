Name:           breeze-accent
Version:        0.1.0
Release:        0
Summary:        Breeze-derived KWin window decoration with a configurable accent outline
License:        GPL-2.0-or-later AND (GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL) AND MIT
URL:            https://github.com/pedazo/breeze-accent
Source0:        %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  gettext-tools
BuildRequires:  kf6-extra-cmake-modules
BuildRequires:  kf6-filesystem
BuildRequires:  kf6-kcmutils
BuildRequires:  ninja
BuildRequires:  cmake(KDecoration3) >= 6.8.3
BuildRequires:  cmake(KF6ColorScheme)
BuildRequires:  cmake(KF6Config)
BuildRequires:  cmake(KF6ConfigWidgets)
BuildRequires:  cmake(KF6CoreAddons)
BuildRequires:  cmake(KF6GuiAddons)
BuildRequires:  cmake(KF6I18n)
BuildRequires:  cmake(KF6KCMUtils)
BuildRequires:  cmake(KF6WidgetsAddons)
BuildRequires:  cmake(Qt6DBus)
BuildRequires:  cmake(Qt6Test)
BuildRequires:  cmake(Qt6Widgets)
Requires:       kwin6 >= 6.8.3

%description
Breeze Accent is an independent KDecoration3 decoration based on KDE Breeze
6.8.3. It adds configurable outlines to server-side window decorations without
replacing Breeze or installing a KWin effect.

%prep
%autosetup

%build
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_INSTALL_PREFIX=%{_prefix} \
  -DKDE_INSTALL_LIBDIR=%{_lib} \
  -DKDE_INSTALL_PLUGINDIR=%{_kf6_plugindir} \
  -DBUILD_TESTING=ON
cmake --build build %{?_smp_mflags}

%install
DESTDIR=%{buildroot} cmake --install build

%check
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure

%files
%license upstream/LICENSES/*
%doc README.md docs
%{_kf6_plugindir}/org.kde.kdecoration3/org.kde.breezeaccent.so
%{_kf6_plugindir}/org.kde.kdecoration3.kcm/kcm_breezeaccent.so

%changelog
