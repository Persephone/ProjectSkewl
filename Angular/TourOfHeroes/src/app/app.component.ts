import { Component, OnInit  } from '@angular/core';
// import { routes } from './app-routing.module';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})

export class AppComponent implements OnInit {
  title = 'Tour of Heroes';
  // navigationList: string[] = [];

  navLinks: Array<{ text: string, path: string }> = [
    {  text: 'Dashboard', path: 'dashboard' },
    {  text: 'Heroes', path: 'heroes' },
  ];

  ngOnInit() {
    // this.getRoutes();
  }

  /*getRoutes(): void {
    routes.forEach(route => {
      if (route.path.length > 0) {
        this.navigationList.push(route.path);
      }
    });
  }*/
}
