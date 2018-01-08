因为在每次重新生成soui工程时，下面的配置会被还原，所以备份在这儿，
重新生成后，需要再次同步到如下位置，才能让UIEngine正常编译。
"$(SOUIPATH)\config\com-def.h"
"$(SOUIPATH)\config\core-def.h"
"$(SOUIPATH)\components\com-cfg.h"