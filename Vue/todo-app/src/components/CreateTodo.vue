<template>
  <div class='ui basic content center aligned segment'>
    <button class='ui teal labeled icon button' v-on:click="openForm" v-show="!isCreating">
        Add Todo
      <i class='add icon'></i>
    </button>
    <div class='ui centered card' v-show="isCreating">
      <div class='content'>
        <div class='ui form'>
          <div class='field'>
            <label>Title</label>
            <input v-model="titleText" type='text' ref='title' defaultValue="" placeholder="Project Title" required>
            <div class="ui pointing red basic label" v-show="titleError">
              Please enter a title
            </div>
          </div>
          <div class='field'>
            <label>Project</label>
            <input v-model="projectText" type='text' ref='project' defaultValue="" placeholder="Project Name" required>
            <div class="ui pointing red basic label" v-show="projectError">
              Please enter a project name
            </div>
          </div>
          <div class='field'>
            <label>Description</label>
            <textarea rows="4" v-model="descriptionText" type='text' ref='project' defaultValue="" required placeholder="Some description..."></textarea>
            <div class="ui pointing red basic label" v-show="descriptionError">
              Please enter a description
            </div>
          </div>
          <div class='ui two button attached buttons'>
            <button class='ui basic blue button' v-on:click="sendForm()">
              Create
            </button>
            <button class='ui basic red button' v-on:click="closeForm()">
              Cancel
            </button>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  data () {
    return {
      titleText: '',
      projectText: '',
      descriptionText: '',
      isCreating: false,

      titleError: false,
      projectError: false,
      descriptionError: false
    }
  },
  computed: {
  },
  methods: {
    openForm () {
      this.isCreating = true
    },
    closeForm () {
      this.isCreating = false
    },
    sendForm () {
      const title = this.titleText
      const project = this.projectText
      const description = this.descriptionText

      if (title && project && description) {
        this.$emit('add-todo', {
          title,
          project,
          description,
          done: false
        })
        this.isCreating = false
      }

      this.titleError = !title
      this.projectError = !project
      this.descriptionError = !description
    }
  }
}
</script>
