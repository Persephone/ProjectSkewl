<template>
  <div id="app">
    <todo-list v-bind:todos="todos"></todo-list>
    <create-todo v-on:add-todo="addTodo"></create-todo>
  </div>
</template>

<script>
import TodoList from './components/TodoList'
import CreateTodo from './components/CreateTodo'

export default {
  name: 'App',
  components: {
    TodoList,
    CreateTodo
  },

  data () {
    return {
      todos: [
        {
          title: 'Todo A',
          project: 'Project A',
          description: 'Project Description',
          done: false
        },
        {
          title: 'Todo B',
          project: 'Project B',
          description: 'Project Description',
          done: true
        },
        {
          title: 'Todo C',
          project: 'Project C',
          description: 'Project Description',
          done: false
        },
        {
          title: 'Todo D',
          project: 'Project D',
          description: 'Project Description',
          done: false
        }
      ]
    }
  },

  methods: {
    addTodo (todo) {
      this.todos.push(todo)
    }
  }
}
</script>

<style>

</style>
