<template>
  <div class="nav">
    <nav>
      <router-link to="/">Home</router-link> |
      <router-link to="/about">About</router-link> |
      <router-link to="/test">Test</router-link>
    </nav>
  </div>
</template>

<script>

</script>

<style lang="scss" scoped>

.nav {
    background-image: linear-gradient(#42A5F5, white);
}

nav{
    display:inline-block;
    margin:13px auto 25px;
    background-color:#42A5F5;
    box-shadow:0 1px 1px #ccc;
    border-radius:2px;
}

nav a{
    display:inline-block;
    padding: 18px 40px;
    color:#fff !important;
    font-weight:bold;
    font-size:16px;
    text-decoration:none !important;
    line-height:1;
    text-transform: uppercase;
    background-color:transparent;

    -webkit-transition:background-color 0.25s;
    -moz-transition:background-color 0.25s;
    transition:background-color 0.25s;
}

nav a:first-child{
    border-radius:2px 0 0 2px;
}

nav a:last-child{
    border-radius:0 2px 2px 0;
}

.router-link-exact-active {
    background-color:#e35885;
}
</style>
