%define beta beta7

Name:           pichi
Version:        0.6.0
Release:        0.1.%{beta}%{?dist}
Summary:        Pichi is a jabber(xmpp) bot writen on C++. It is based on sulci facilities.
Group:          Applications/Internet
License:        GPLv2
URL:            http://code.google.com/p/pichi
Source0:        http://pichi.googlecode.com/files/%{name}-%{version}.%{beta}.tar.gz

BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  cmake
BuildRequires:  boost-devel
BuildRequires:  libcurl-devel
BuildRequires:  sqlite-devel
BuildRequires:  gloox-devel
BuildRequires:  tinyxml-devel

Requires:       boost-regex
Requires:       boost-program-options
Requires:       libcurl
Requires:       sqlite
Requires:       gloox
Requires:       tinyxml


%description
Pichi is a jabber(xmpp) bot writen on C++. It is based on sulci facilities.


%prep
%setup -q -n %{name}-%{version}.%{beta}


%build
%{cmake}
make %{?_smp_mflags}


%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
%doc README.md
%{_bindir}/pichi
%{_datadir}/pichi/languages/*.xml
%{_datadir}/pichi/config/pichi.xml

%changelog
* Sun Nov 21 2010 Alexey Kasyanchuk <deg@uruchie.org> - 0.6.0-0.1.beta3
- Initial package
 
