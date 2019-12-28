# 蓝牙协议栈组件 for AmebaD/AmebaZ2

该组件包含了原厂SDK提供的蓝牙协议栈相关API以及profile

## 使用说明

bt.c中定义了以下三个弱定义函数，需要用户在应用层根据具体应用需求来实现相关功能，可参考mxos demo中的示例。

```C
/** Pre-handle all the BT GAP MSGs */
WEAK void mbt_app_handle_gap_msg( T_IO_MSG *p_gap_msg )
{

}

/** Initialize GATT profiles */
WEAK void mbt_app_le_profile_init( void )
{

}

/** Initialize GAP scan/adv/connect/bond... parameters */
WEAK void mbt_app_le_gap_init( void )
{

}
```
