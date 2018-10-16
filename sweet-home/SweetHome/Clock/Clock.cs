using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SweetHome
{
    class Clock
    {
        public Clock(Control control, int timeZone)
        {
            var timer = new System.Threading.Timer(t => DisplayTime(control, timeZone), null, 0, 60);
         }

        public void DisplayTime(Control control, int timeZone)
        {
            DateTime t = DateTime.UtcNow.AddHours(timeZone);
            control.Invoke((MethodInvoker)(() => control.Text = t.ToString("HH:mm:ss")));

        }
    }
}
