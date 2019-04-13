<template>
<div class="column">
    <div class="ui segment">
  <div class='ui centered card'>
    <div class="content" v-show="!isEditing">
      <div class='header ui blue ribbon label'>
          {{ todo.title }}
      </div>
      <div class=''>
          {{ 'Project: ' + todo.project }}
      </div>
      <div>
          {{ todo.description }}
      </div>
      <div class='extra content'>
          <v-tooltip bottom>
      <template v-slot:activator="{ on }">
        <span @mouseover="mouseOver()" class='right floated edit icon' v-on:click="showForm()">
          <i class='edit icon'></i>
        </span>
        </template>
      <span>Tooltip</span>
    </v-tooltip>
        <span @mouseover="mouseOver()" class='right floated trash icon' v-on:click="deleteTodo(todo)">
          <i class='trash icon'></i>
        </span>
      </div>
    </div>
    <div class="content" v-show="isEditing">
      <div class='ui form'>
        <div class='field'>
          <label>Title</label>
          <input type='text' v-model="todo.title" >
        </div>
        <div class='field'>
          <label>Project</label>
          <input type='text' v-model="todo.project" >
        </div>
        <div class='field'>
          <label>Description</label>
          <textarea rows="4" v-model="todo.description" type='text' ref='project' defaultValue="" required placeholder="Some description..."></textarea>
        </div>
        <div class='ui two button attached buttons'>
          <button class='ui basic blue button' v-on:click="hideForm()">
            Save
          </button>
        </div>
      </div>
    </div>
    <div class='ui bottom attached green basic button' v-show="!isEditing && todo.done" v-on:click="toggleTodo(todo)">
        Completed
    </div>
    <div class='ui bottom attached red basic button' v-show="!isEditing && !todo.done" v-on:click="toggleTodo(todo)">
        Pending
    </div>
  </div>
  </div>
</div>
</template>

<script type="text/javascript">

export default {
  props: ['todo'],
  data () {
    return {
      isEditing: false,
      hoverActive: false
    }
  },
  methods: {
    showForm () {
      this.isEditing = true
    },
    hideForm () {
      this.isEditing = false
    },
    deleteTodo (todo) {
      this.$emit('delete-todo', todo)
    },
    toggleTodo (todo) {
      this.$emit('toggle-todo', todo)
    },
    mouseOver () {
      this.hoverActive = !this.hoverActive
    }
  }
}

</script>

<style>
.ui.segment {
    background: lightblue;
}

</style>
