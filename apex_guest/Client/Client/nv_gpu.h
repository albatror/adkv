#pragma once
#include <cinttypes>
#include <Windows.h>
#include <d3dkmthk.h>
#include <string>
#include <algorithm>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "advapi32.lib")

#define NV01_DEVICE_0      (0x80U) /* finn: Evaluated from "NV0080_ALLOC_PARAMETERS_MESSAGE_ID" */
#define NV0080_ALLOC_PARAMETERS_MESSAGE_ID (0x0080U)

#define NV20_SUBDEVICE_0      (0x2080U) /* finn: Evaluated from "NV2080_ALLOC_PARAMETERS_MESSAGE_ID" */
#define NV2080_ALLOC_PARAMETERS_MESSAGE_ID (0x2080U)

#define NV2080_GPU_MAX_GID_LENGTH (0x000000100ULL)
#define NV2080_CTRL_CMD_GPU_GET_GID_INFO      (0x2080014aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_GID_INFO_PARAMS_MESSAGE_ID" */
#define NV2080_CTRL_CMD_GPU_GET_NAME_STRING   (0x20800110U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS_MESSAGE_ID" */

#define NV2080_GPU_MAX_NAME_STRING_LENGTH                  (0x0000040U)
#define NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_ASCII   (0x00000000U)
#define NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_UNICODE (0x00000001U)

#define NV01_ROOT_CLIENT (0x00000041)

#if defined(__GNUC__) || defined(__clang__) || defined(NV_QNX)
#define NV_DECLARE_ALIGNED(TYPE_VAR, ALIGN) TYPE_VAR __attribute__ ((aligned (ALIGN)))
#elif defined(_MSC_VER)
#define NV_DECLARE_ALIGNED(TYPE_VAR, ALIGN) __declspec(align(ALIGN)) TYPE_VAR
#elif defined(__arm)
#define NV_DECLARE_ALIGNED(TYPE_VAR, ALIGN) __align(ALIGN) TYPE_VAR
#endif

#define NV_OK 0x00000000
#define NV_ERR_BUFFER_TOO_SMALL 0x00000002
#define NV_ERR_OPERATING_SYSTEM 0x00000059

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

namespace nvre
{
    using NvU32 = std::uint32_t;
    using NvU64 = std::uint64_t;
    using NvU16 = std::uint16_t;
    using NvV32 = std::uint32_t;
    using NvU8 = std::uint8_t;

    typedef struct
    {
        NvU32 val;
    } NvHandle;

    typedef struct NV0041_ALLOC_PARAMETERS {
        NvHandle hRoot;
    } NV0041_ALLOC_PARAMETERS;

    typedef struct NV0080_ALLOC_PARAMETERS {
        NvU32    deviceId;
        NvHandle hClientShare;
        NvHandle hTargetClient;
        NvHandle hTargetDevice;
        NvV32    flags;
        NV_DECLARE_ALIGNED(NvU64 vaSpaceSize, 8);
        NV_DECLARE_ALIGNED(NvU64 vaStartInternal, 8);
        NV_DECLARE_ALIGNED(NvU64 vaLimitInternal, 8);
        NvV32    vaMode;
    } NV0080_ALLOC_PARAMETERS;

    typedef struct NV2080_ALLOC_PARAMETERS {
        NvU32 subDeviceId;
    } NV2080_ALLOC_PARAMETERS;

    typedef struct NV2080_CTRL_GPU_GET_GID_INFO_PARAMS {
        NvU32 index;
        NvU32 flags;
        NvU32 length;
        NvU8  data[NV2080_GPU_MAX_GID_LENGTH];
    } NV2080_CTRL_GPU_GET_GID_INFO_PARAMS;

    typedef struct NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS {
        NvU32 gpuNameStringFlags;
        union {
            NvU8  ascii[NV2080_GPU_MAX_NAME_STRING_LENGTH];
            NvU16 unicode[NV2080_GPU_MAX_NAME_STRING_LENGTH];
        } gpuNameString;
    } NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS;

    typedef struct
    {
        NvHandle  hRoot;
        NvHandle  hObjectParent;
        NvHandle  hObjectOld;
        NvV32     status;
    } NVOS00_PARAMETERS;

    typedef struct
    {
        NvHandle hRoot;             // [IN]  client handle
        NvHandle hObjectParent;     // [IN]  parent handle of the new object
        NvHandle hObjectNew;        // [IN]  new object handle
        NvV32    hClass;            // [IN]  class num of the new object
        NvU32    paramSize;         // [IN]  size in bytes of the RM alloc parameters
        NvV32    status;            // [OUT] status
    } NVOS62_PARAMETERS;

    typedef struct
    {
        NvHandle hClient;       // [IN]  client handle
        NvHandle hObject;       // [IN]  object handle
        NvV32    cmd;           // [IN]  control command ID
        NvU32    paramsSize;    // [IN]  size in bytes of the RM Control parameters
        NvV32    status;        // [OUT] status
    } NVOS63_PARAMETERS;

    enum {
        NVOS_TYPE_FREE = 0x500000C,
        NVOS_TYPE_ALLOC = 0x500002A,
        NVOS_TYPE_CONTROL = 0x500002B
    };

    struct nv_header
    {
        NvU32 magic;
        NvU32 flags;
        NvU32 total_size;
        NvU32 nvos_magic;
        NvU32 nvos_type;
        NvU8 reserved[0x1C];
    };

    struct nv_free : nv_header, NVOS00_PARAMETERS {};

    template<typename T>
    struct nv_alloc : nv_header, NVOS62_PARAMETERS
    {
        T params;
    };

    template<typename T>
    struct nv_control : nv_header, NVOS63_PARAMETERS
    {
        T params;
    };

    inline NTSTATUS d3dkmt_escape(NvU32 adapter, D3DKMT_ESCAPETYPE type, void* private_data, NvU32 size)
    {
        D3DKMT_ESCAPE data = { 0 };
        data.hAdapter = adapter;
        data.Type = type;
        data.pPrivateDriverData = (void*)private_data;
        data.PrivateDriverDataSize = size;

        return D3DKMTEscape(&data);
    }

    template<typename Nv>
    inline void setup_header(Nv& r, NvU32 type)
    {
        r.magic = 'NVDA';
        r.flags = 0x10002;
        r.total_size = sizeof(Nv);
        r.nvos_magic = 'NV**';
        r.nvos_type = type;

        memset(r.reserved, 0, sizeof(r.reserved));
    }

    template<typename T>
    inline nvre::NvV32 alloc(nv_alloc<T>& r,
        NvU32 adapter,
        NvHandle root,
        NvHandle object_parent,
        NvHandle object_new,
        NvV32 class_handle)
    {
        setup_header(r, NVOS_TYPE_ALLOC);

        r.hRoot = root;
        r.hObjectParent = object_parent;
        r.hObjectNew = object_new;
        r.hClass = class_handle;
        r.paramSize = sizeof(T);
        r.status = NV_OK;

        if (NT_SUCCESS(d3dkmt_escape(adapter, D3DKMT_ESCAPE_DRIVERPRIVATE, &r, sizeof(r)))) return r.status;
        return NV_ERR_OPERATING_SYSTEM;
    }

    template<typename T>
    inline nvre::NvV32 control(nv_control<T>& r,
        NvU32 adapter,
        NvHandle client,
        NvHandle object,
        NvV32 cmd)
    {
        setup_header(r, NVOS_TYPE_CONTROL);

        r.hClient = client;
        r.hObject = object;
        r.cmd = cmd;
        r.paramsSize = sizeof(T);
        r.status = NV_OK;

        if (NT_SUCCESS(d3dkmt_escape(adapter, D3DKMT_ESCAPE_DRIVERPRIVATE, &r, sizeof(r)))) return r.status;
        return NV_ERR_OPERATING_SYSTEM;
    }

    inline NvV32 free_handle(NvU32 adapter, NvHandle root, NvHandle object_parent, NvHandle object_old)
    {
        nv_free r;
        setup_header(r, NVOS_TYPE_FREE);

        r.hRoot = root;
        r.hObjectParent = object_parent;
        r.hObjectOld = object_old;
        r.status = NV_OK;

        if (NT_SUCCESS(d3dkmt_escape(adapter, D3DKMT_ESCAPE_DRIVERPRIVATE, &r, sizeof(r)))) return r.status;
        return NV_ERR_OPERATING_SYSTEM;
    }

    inline NvV32 alloc_root_handle(NvU32 adapter, NvHandle* root)
    {
        nv_alloc<NV0041_ALLOC_PARAMETERS> r;
        memset(&r.params, 0, sizeof(r.params));

        const auto result = alloc(r, adapter, { 0 }, { 0 }, { 0 }, NV01_ROOT_CLIENT);
        if (result == NV_OK)
            *root = r.params.hRoot;

        return result;
    }

    inline NvV32 alloc_nv0080_handle(NvU32 adapter, NvHandle root, NvHandle object_new)
    {
        nv_alloc<NV0080_ALLOC_PARAMETERS> r;
        memset(&r.params, 0, sizeof(r.params));

        return alloc(r, adapter, root, root, object_new, NV0080_ALLOC_PARAMETERS_MESSAGE_ID);
    }

    inline NvV32 alloc_nv2080_handle(NvU32 adapter, NvHandle root, NvHandle object_parent, NvHandle object_new)
    {
        nv_alloc<NV2080_ALLOC_PARAMETERS> r;
        memset(&r.params, 0, sizeof(r.params));

        return alloc(r, adapter, root, object_parent, object_new, NV2080_ALLOC_PARAMETERS_MESSAGE_ID);
    }

    inline NvV32 nv2080_ctrl_gpu_get_gid_info(NvU32 adapter, NvHandle client, NvHandle object, std::string& info)
    {
        nv_control< NV2080_CTRL_GPU_GET_GID_INFO_PARAMS> r;
        memset(&r.params, 0, sizeof(r.params));

        const auto result = control(r, adapter, client, object, NV2080_CTRL_CMD_GPU_GET_GID_INFO);
        if (result == NV_OK)
        {
            info.resize(r.params.length);
            memcpy((void*)info.data(), r.params.data, r.params.length);
        }

        return result;
    }

    inline NvV32 nv2080_ctrl_gpu_get_name(NvU32 adapter, NvHandle client, NvHandle object, std::string& name)
    {
        nv_control< NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS> r;
        memset(&r.params, 0, sizeof(r.params));

        r.params.gpuNameStringFlags = NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_ASCII;

        const auto result = control(r, adapter, client, object, NV2080_CTRL_CMD_GPU_GET_NAME_STRING);
        if (result == NV_OK)
            name = (const char*)r.params.gpuNameString.ascii;

        return result;
    }

    inline std::string get_gpu_uuid()
    {
        D3DKMT_ENUMADAPTERS2 enum_adapters = { 0 };
        NTSTATUS status;

        status = D3DKMTEnumAdapters2(&enum_adapters);
        if (!NT_SUCCESS(status))
            return "";

        enum_adapters.pAdapters = (D3DKMT_ADAPTERINFO*)malloc(
            sizeof(D3DKMT_ADAPTERINFO) * enum_adapters.NumAdapters
        );

        if (!enum_adapters.pAdapters)
            return "";

        status = D3DKMTEnumAdapters2(&enum_adapters);
        if (!NT_SUCCESS(status))
        {
            free(enum_adapters.pAdapters);
            return "";
        }

        nvre::NvHandle root;
        const nvre::NvHandle nv0080_handle = { 0xBBAAF2A0 };
        const nvre::NvHandle nv2080_handle = { 0xBBAAF2A0 + 0x10 };

        std::string gpu_uuid;

        for (UINT i = 0; i < enum_adapters.NumAdapters; i++)
        {
            const auto adapter = enum_adapters.pAdapters[i].hAdapter;

            if (nvre::alloc_root_handle(adapter, &root) != NV_OK) continue;

            do
            {
                if (nvre::alloc_nv0080_handle(adapter, root, nv0080_handle) != NV_OK) break;
                if (nvre::alloc_nv2080_handle(adapter, root, nv0080_handle, nv2080_handle) != NV_OK) break;
                if (nvre::nv2080_ctrl_gpu_get_gid_info(adapter, root, nv2080_handle, gpu_uuid) != NV_OK) break;
            } while (false);

            nvre::free_handle(adapter, root, nv0080_handle, nv2080_handle);
            nvre::free_handle(adapter, root, root, nv0080_handle);
            nvre::free_handle(adapter, root, root, root);

            if (!gpu_uuid.empty())
                break;
        }

        free(enum_adapters.pAdapters);
        return gpu_uuid;
    }
};
