using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace WebAPI.Models
{
    public class BMIModel
    {
        public double Height { get; set; }
        public double Weight { get; set; }

        public double Calculate => Height * Weight;
    }
}