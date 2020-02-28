#include <dfs_posix.h>
#include "type.h"

int request_firmware(const struct firmware **fw, const char *file)
{
    struct stat st;
    int result;
    struct firmware *firmware;

    result = stat(file, &st);
    if (result < 0)
    {
        rt_kprintf("firmware:[%s] not found!\n", file);
        return -RT_ERROR;
    }

    firmware = (struct firmware *)rt_malloc(sizeof(*firmware));
    if (firmware == RT_NULL)
        return -RT_ENOMEM;

    firmware->data = RT_NULL;
    firmware->size = st.st_size;
    firmware->fd = open(file, O_RDONLY, 0);

    *fw = firmware;

    return RT_EOK;
}

void release_firmware(const struct firmware *fw)
{
    close(fw->fd);
    rt_free((void *)fw);
}

void get_firmware_data(const struct firmware *fw, u8 *buf, u32 len)
{
    read(fw->fd, buf, len);
}
