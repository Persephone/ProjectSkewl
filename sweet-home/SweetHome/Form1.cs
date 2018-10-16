using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using Imgur.API;
using Imgur.API.Authentication.Impl;
using Imgur.API.Endpoints.Impl;
using Imgur.API.Models;

namespace SweetHome 
{
    public partial class Form1 : Form {
        public Form1() 
        {
            InitializeComponent();
        }

        void Loaded(object sender, System.EventArgs e)
        {
            Clock denmark = new Clock(label1, 2);
            Clock england = new Clock(label2, 1);
        }
        
        private async void GetImage_Click(object sender, EventArgs e)
        {
            // MessageBox.Show("Uploading");
             await UploadImage();
        }

        public async Task UploadImage() 
        {
            try
            {
                var client = new ImgurClient("f70d50e368f297c", "234c95c5baff22381d4acbbdb8d6033a834e25a0");
                var endpoint = new ImageEndpoint(client);
                IImage image;
                using (var fs = new FileStream(@"/home/inaba/Pictures/Screenshot_20171117_114419.png", FileMode.Open))
                {
                    image = await endpoint.UploadImageStreamAsync(fs);
                }

                MessageBox.Show("Image uploaded. Image Url: " + image.Link);
            }
            catch (ImgurException imgurEx)
            {
                MessageBox.Show("An error occurred uploading an image to Imgur.");
                MessageBox.Show(imgurEx.Message);
            }
        }
    }
}
