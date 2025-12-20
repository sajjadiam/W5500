void DHCP_Init(unsigned char socket_num); //(اختصاص یک سوکت خاص به DHCP)
void DHCP_Run(void); //(باید در حلقه اصلی برنامه مدام صدا زده شود تا تایم‌اوت‌ها را چک کند)
void DHCP_ParseMessage(void); //(تحلیل پکت دریافتی از سرور)
