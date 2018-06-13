changecom(`%')

% Copyright (C) 2017 Daniel Page <dan@phoo.org>
%
% Use of this source code is restricted per the CC BY-SA license, a copy of
% which can be found via http://creativecommons.org (and should be included 
% as LICENSE.txt within the associated archive or repository).

define(`CONF',`ifelse(`$#',`0',``CONF'',`$#',`1',SCALE_CONF_$1,`$#',`2',SCALE_CONF_$2_$1)')
define(`USER',`ifelse(`$#',`0',``USER'',`$#',`1',SCALE_USER_$1,`$#',`2',SCALE_USER_$2_$1)')

