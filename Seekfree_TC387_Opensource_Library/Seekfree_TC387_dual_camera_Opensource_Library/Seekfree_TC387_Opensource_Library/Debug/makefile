################################################################################
# 鑷姩鐢熸垚鐨勬枃浠躲�備笉瑕佺紪杈戯紒
################################################################################

-include ../makefile.init

RM := rm -rf

# 姝ゅ瀹氫箟浜嗗弬涓庣敓鎴愮殑鎵�鏈夋簮
-include sources.mk
-include user/subdir.mk
-include libraries/zf_driver/subdir.mk
-include libraries/zf_device/subdir.mk
-include libraries/zf_components/subdir.mk
-include libraries/zf_common/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/_PinMap/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/_Lib/DataHandling/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/_Impl/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Stm/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Src/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Scu/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Qspi/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Qspi/SpiMaster/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Port/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Pms/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Mtu/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Gtm/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Gtm/Atom/Pwm/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Gpt12/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Gpt12/IncrEnc/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Flash/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Evadc/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Evadc/Adc/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Dma/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Dma/Dma/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Cpu/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Cpu/Irq/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Ccu6/Timer/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Ccu6/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Asclin/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC38A/Tricore/Asclin/Asc/subdir.mk
-include libraries/infineon_libraries/Service/CpuGeneric/SysSe/Math/subdir.mk
-include libraries/infineon_libraries/Service/CpuGeneric/SysSe/Bsp/subdir.mk
-include libraries/infineon_libraries/Service/CpuGeneric/StdIf/subdir.mk
-include libraries/infineon_libraries/Service/CpuGeneric/If/subdir.mk
-include libraries/infineon_libraries/Infra/Ssw/TC38A/Tricore/subdir.mk
-include libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/subdir.mk
-include libraries/infineon_libraries/Configurations/subdir.mk
-include code/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++M_DEPS)),)
-include $(C++M_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CCM_DEPS)),)
-include $(CCM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXXM_DEPS)),)
-include $(CXXM_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Seekfree_TC387_Opensource_Library
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# 灏嗚繖浜涘伐鍏疯皟鐢ㄧ殑杈撳叆鍜岃緭鍑烘坊鍔犲埌鏋勫缓鍙橀噺 
SECONDARY_SIZE += \
Seekfree_TC387_Opensource_Library.siz \


# 鎵�鏈夌洰鏍�
all: main-build

# 涓绘瀯寤虹洰鏍�
main-build: Seekfree_TC387_Opensource_Library.elf secondary-outputs

# 宸ュ叿璋冪敤
Seekfree_TC387_Opensource_Library.elf: $(OBJS) makefile $(OPTIONAL_TOOL_DEPS) .Seekfree_TC387_Opensource_Library.elf.opt
	@echo '姝ｅ湪鏋勫缓鐩爣锛� $@'
	@echo '姝ｅ湪璋冪敤锛� TASKING Linker'
	cctc -f .Seekfree_TC387_Opensource_Library.elf.opt
	@echo '宸茬粨鏉熸瀯寤虹洰鏍囷細 $@'
	@echo ' '

.Seekfree_TC387_Opensource_Library.elf.opt:
	@argfile .Seekfree_TC387_Opensource_Library.elf.opt --lsl-file="../Lcf_Tasking_Tricore_Tc.lsl" -Wl-Oc -Wl-OL -Wl-Ot -Wl-Ox -Wl-Oy "C:\Users\Wiz\Desktop\ZF\TC387_Library\Seekfree_TC387_Opensource_Library\Seekfree_TC387_dual_camera_Opensource_Library\Seekfree_TC387_Opensource_Library\libraries\zf_device\zf_device_config.a" -Wl--map-file="Seekfree_TC387_Opensource_Library.map" -Wl-mc -Wl-mf -Wl-mi -Wl-mk -Wl-ml -Wl-mm -Wl-md -Wl-mr -Wl-mu --no-warnings= -Wl--error-limit=42 --fp-model=3 -lrt -lcs_fpu --lsl-core=vtc --exceptions --strict --anachronisms --force-c++ -Ctc38x -o"Seekfree_TC387_Opensource_Library.elf" -Wl-o"Seekfree_TC387_Opensource_Library.hex:IHEX" $(OBJS)

.PHONY: .Seekfree_TC387_Opensource_Library.elf.opt

Seekfree_TC387_Opensource_Library.siz: Seekfree_TC387_Opensource_Library.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo '姝ｅ湪璋冪敤锛� Print Size'
	elfsize  "Seekfree_TC387_Opensource_Library.elf"
	@echo '宸茬粨鏉熸瀯寤猴細 $@'
	@echo ' '

# 鍏朵粬鐩爣
clean:
	-$(RM) Seekfree_TC387_Opensource_Library.siz Seekfree_TC387_Opensource_Library.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build

-include ../makefile.targets
