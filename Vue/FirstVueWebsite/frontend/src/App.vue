<template>
  <div id="app">
    <NavigationBar/>
    <div class="content">
      <router-view/>
    </div>
    <Footer/>
  </div>
</template>

<script>
import NavigationBar from './components/Nav.vue';
import Footer from './components/Footer.vue';

export default {
  name: 'app',
  components: {
    NavigationBar,
    Footer,
  },
};
</script>

<style lang="scss">

@import 'src/assets/css/main.scss';

#app {
  display: flex;
  flex-direction: column;
  height: 100%;

  font-family: 'Avenir', Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  color: #2c3e50;
}
</style>
