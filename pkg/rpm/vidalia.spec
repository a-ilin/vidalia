# $Id: $
#

#%package vidalia
Name		: vidalia
Version		: 0.0.8
Release		: 3
#
License		: GPL
Summary		: The Tor-Gui for Linux
Group		: Applications/Internet

URL		: http://www.vidalia-project.net
Vendor		: Matt Edman, Justin Hipple
Packager	: Oliver Niesner <digi_news@gmx.de> 

Source0		: %{name}-%{version}.tar.gz

#AutoReq     	: No
#AutoProv	: No

Requires        : tor >= 0.1.1.15
BuildRequires	: qt4
#BuildRoot	: /var/tmp/%{name}-buildroot

%description

Vidalia is an easy to use Tor-Controller Gui

%prep
rm -rf %{buildroot}

#######################################################################
# setup macro
# -a num  : Only unpack source number after changing to the directory
# -b num  : Only unpack source number before changing to the directory
# -c      : Create directory before unpacking.
# -D      : Do not delete the directory before unpacking
# -n name : Name the directory as name
# -q      : Run quiety with minimum output
# -T      : Disable the automatic unpacking of the archives.
#######################################################################
%setup 
%build
./configure
make
#########################################################
# Common Red Hat RPM macros (rpm --showrc for more info)
# {_sourcedir} : /usr/src/redhat/SOURCES
# {_builddir}  : /usr/src/redhat/BUILD
# {_tmppath}   : /var/tmp
# {_libdir}    : /usr/lib
# {_bindir}    : /usr/bin
# {_datadir}   : /usr/share
# {_mandir}    : /usr/share/man
# {_docdir}    : /usr/share/doc
# {buildroot}
# {name}
# {version}
# {release}
#########################################################

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
mkdir -p $RPM_BUILD_ROOT/usr/local/man/man1
#install -s -m 755 eject $RPM_BUILD_ROOT/usr/bin/eject
#install -m 644 eject.1 $RPM_BUILD_ROOT/usr/man/man1/eject.1

%makeinstall
#%post

%clean
rm -rf %{buildroot}

%files
/usr/local/bin/vidalia 
/usr/local/man/man1/vidalia.1
#####################################################
# defattr sets the default attributes for all files
#####################################################
%defattr(-, root, root)

%changelog
* Tue Oct 11 2006 Oliver Niesner <digi_news@gmx.de> 0.0.8-1
- initial release
