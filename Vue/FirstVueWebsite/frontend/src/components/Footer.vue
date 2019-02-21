<template>
  <div id="footer">
      <div>
      <img alt="Vue logo" src="../assets/logo.png">
      </div>
      <div>
        <p>Test test </p>
      </div>
      <div>
        <p>Test 2</p>
      </div>
  </div>
</template>

<style lang="scss" scoped>

#footer {
    display: flex;
    flex-shrink: 0;
    flex-direction: row;            /* default value; can be omitted */
    flex-wrap: nowrap;              /* default value; can be omitted */
    justify-content: space-between; /* switched from default (flex-start, see below) */
    /* Prevent Chrome, Opera, and Safari from letting these items shrink to smaller than their content's default minimum size. */
    padding: 20px;
    background: #42A5F5;
    color: white;

    div {
        width: 33%;
        height: 35px;
    }

    div img {
        max-width: 100%;
        max-height: 100%;
        object-fit: contain;
    }
  }
</style>
