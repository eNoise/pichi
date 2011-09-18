%define preversion b2

Name:           pichi
Version:        0.7.0
Release:        0.1.%{preversion}%{?dist}
#Release:        1%{?dist}
Summary:        Pichi is a jabber(xmpp) bot written in C++. It is based on the sulci abilities.
Group:          Applications/Internet
License:        GPLv2
URL:            http://code.google.com/p/pichi
Source0:        http://pichi.googlecode.com/files/%{name}-%{version}%{preversion}.tar.gz
#Source0:        http://pichi.googlecode.com/files/%{name}-%{version}.tar.gz

BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  cmake
BuildRequires:  boost-devel
BuildRequires:  libcurl-devel
BuildRequires:  sqlite-devel
# pathed gloox 1.0.1dev included
#BuildRequires:  gloox-devel
BuildRequires:  tinyxml-devel
BuildRequires:  openssl-devel
BuildRequires:  lua-devel

Requires:       boost-regex
Requires:       boost-program-options
Requires:       libcurl
Requires:       sqlite
# pathed gloox 1.0.1dev included
#Requires:       gloox
Requires:       tinyxml
Requires:       openssl
Requires:       lua


%description
Pichi is a jabber(xmpp) bot written in C++. It is based on the sulci abilities.


%prep
#%setup -q -n %{name}-%{version}
%setup -q -n %{name}-%{version}%{preversion}

%build
%{cmake}
make %{?_smp_mflags}


%check
make test


%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
%doc README.md COPYING TODO ChangeLog.txt ChangeLog_ru.txt
%{_bindir}/pichi
%{_datadir}/pichi/languages/*.xml
%{_datadir}/pichi/lua/*.lua
%{_datadir}/pichi/config/pichi.xml

%changelog
* Sat Sep 01 2011 Alexey Kasyanchuk <deg@uruchie.org> - 0.7.0-0.1.beta2
- 0.7.0 beta2

* Sat Dec 25 2010 Alexey Kasyanchuk <deg@uruchie.org> - 0.6.1-1
- Release 0.6.1

* Sun Nov 21 2010 Alexey Kasyanchuk <deg@uruchie.org> - 0.6.0-0.1.beta3
- Initial package

