
## Make sure KDEDIRS is set
if ( ! $?KDEDIRS ) setenv KDEDIRS /usr

## When/if using prelinking, avoids use of kdeinit
if ( ! $?KDE_IS_PRELINKED && -f /etc/sysconfig/prelink ) then
  set prelink=`grep 'PRELINKING=' /etc/sysconfig/prelink | sed -e 's/.*=//'`
  if ( X$prelink == Xyes ) then
    setenv KDE_IS_PRELINKED 1
  endif
endif

## adjust QT_PLUGIN_PATH
foreach libdir ( /usr/lib64 /usr/lib )
#if ( -f ${libdir}/libkdecore.so.5 ) then
if ( $?QT_PLUGIN_PATH ) then
  if ( "${QT_PLUGIN_PATH}" !~ *${libdir}/kde4/plugins* ) then
    setenv QT_PLUGIN_PATH ${QT_PLUGIN_PATH}:${libdir}/kde4/plugins
  endif
else
  setenv QT_PLUGIN_PATH ${libdir}/kde4/plugins
endif
#endif
end

# Fix the user-places.xbel error pop at first user login
if ( ! -d "${HOME}/.local/share" && -w "${HOME}" ) then
  mkdir -p "${HOME}/.local/share"
endif

unsetenv libdir
