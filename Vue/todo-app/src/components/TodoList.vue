<template>
  <div class="main">
    <div class="ui two column grid">
        <todo v-on:toggle-todo="toggleTodo" v-on:delete-todo="deleteTodo" v-for="todo in todos" :key="todo.title" v-bind:todo="todo"></todo>
    </div>
    <div class="info">
      <p>Completed Tasks: {{todos.filter(todo => { return todo.done === true }).length}}</p>
      <p>Pending Tasks: {{todos.filter(todo => { return todo.done === false }).length}}</p>
    </div>
  </div>
</template>

<script type = "text/javascript">

import Todo from './Todo'

export default {
  props: ['todos'],
  components: {
    Todo
  },
  methods: {
    deleteTodo (todo) {
      const todoIndex = this.todos.indexOf(todo)
      this.todos.splice(todoIndex, 1)
    },
    toggleTodo (todo) {
      todo.done = !todo.done
    }
  }
}

</script>

<style>

.main {
    margin: 20px;
}

.info {
    margin-top: 15px;
    text-align: center;
    font-size: 20px;
}
</style>
