%define name authforce
%define version 0.9.6
%define release 1
Name: %{name}
Version: %{version}
Release: %{release}
Summary: HTTP authentication brute forcer
Source: %{name}-%{version}.tar.gz
URL: http://kapheine.hypa.net/authforce
Group: Networking/WWW
BuildRoot: %{_tmppath}/%{name}-buildroot
Copyright: GPL
Packager: Panagiotis Issaris <takis@lumumba.luc.ac.be>
Prefix: %{_prefix}

%description
Authforce is an HTTP authentication brute forcer. Using various methods,
it attempts brute force username and password pairs for a site. It has 
the ability to try common username and passwords, username derivations,
and common username/password pairs. It is used to both test the security
of your site and to prove the insecurity of HTTP authentication based on
the fact that users just don't pick good passwords.

%description -l nl 
Authforce is een programma dat HTTP authenticatie probeert te verkrijgen
door brute kracht. Op verschillende methoden probeert het paren gebruikersnamen
en toegangswoorden uit op een site. Het geeft de mogelijkheid gebruik
te maken van veelgebruikte gebruikersnamen en toegangswoorden, veelgebruikte
paren van gebruikersnamen en toegangswoorden en toegangswoorden afgeleid van
de gebruikersnaam. Het wordt gebruikt om de veiligheid van uw site te testen
en de onveiligheid van HTTP authenticatie aan te tonen gebaseerd op het feit
dat gebruikers simpelweg geen goede toegangswoorden kiezen.

%prep
%setup

%build
CFLAGS="-O2"
%ifarch i586
CFLAGS="-O2 -mpentium -march=pentium"
%endif
%ifarch i686
CFLAGS="-O2 -mpentiumpro -march=pentiumpro"
%endif

%configure

%make

%install
rm -rf $RPM_BUILD_ROOT

%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,0755)
%doc ABOUT-NLS README NEWS COPYING AUTHORS ChangeLog TODO THANKS authforcerc.sample 
%{prefix}/bin/authforce
%{prefix}/share/*/*
