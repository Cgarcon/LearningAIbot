#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Declare the main components of the app
GtkWidget *chat_window;
GtkWidget *chat_text_view;
GtkWidget *user_input_entry;
GtkWidget *send_button;

// Structure to store conversation pairs (input-output)
#define MAX_CONVERSATIONS 1000000
#define MAX_INPUT_LENGTH 200
#define MAX_RESPONSE_LENGTH 500

typedef struct {
    char input[MAX_INPUT_LENGTH];
    char response[MAX_RESPONSE_LENGTH];
} Conversation;

Conversation conversations[MAX_CONVERSATIONS];
int conversation_count = 0;

// Simple rule-based chatbot response function with learning feature
const char* get_bot_response(const char* user_input) {
    for (int i = 0; i < conversation_count; i++) {
        if (strstr(user_input, conversations[i].input) != NULL) {
            return conversations[i].response;
        }
    }
    return NULL;
}

// Update the chat window with new message
void update_chat_window(const char* message) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_text_view));
    gtk_text_buffer_insert_at_cursor(buffer, message, -1);
    gtk_text_buffer_insert_at_cursor(buffer, "\n", -1);
}

// Function to store new conversation pairs (learn new input-output)
void learn_new_response(const char* user_input, const char* bot_response) {
    if (conversation_count < MAX_CONVERSATIONS) {
        strncpy(conversations[conversation_count].input, user_input, MAX_INPUT_LENGTH);
        strncpy(conversations[conversation_count].response, bot_response, MAX_RESPONSE_LENGTH);
        conversation_count++;
        update_chat_window("Bot: Thank you for teaching me!");
    } else {
        update_chat_window("Bot: My memory is full. I can't learn new responses.");
    }
}

// Event handler for the "send" button
void on_send_button_clicked(GtkWidget *widget, gpointer data) {
    const char *user_input = gtk_entry_get_text(GTK_ENTRY(user_input_entry));

    // Display user input in the chat window
    update_chat_window("You: ");
    update_chat_window(user_input);

    // Get the bot response
    const char *bot_response = get_bot_response(user_input);
    if (bot_response != NULL) {
        // Display bot response if it's found in the conversation list
        update_chat_window("Bot: ");
        update_chat_window(bot_response);
    } else {
        // If bot doesn't understand, ask the user for a response
        update_chat_window("Bot: Sorry, I don't understand. Can you teach me?");
        
        // Ask for the bot's new response
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Teach Me!", GTK_WINDOW(chat_window), GTK_DIALOG_MODAL,
                                                       "OK", GTK_RESPONSE_OK, NULL);
        GtkWidget *entry = gtk_entry_new();
        gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), entry);
        gtk_widget_show_all(dialog);
        
        gint result = gtk_dialog_run(GTK_DIALOG(dialog));
        
        if (result == GTK_RESPONSE_OK) {
            const char *new_response = gtk_entry_get_text(GTK_ENTRY(entry));
            learn_new_response(user_input, new_response);
        }
        
        gtk_widget_destroy(dialog);
    }

    // Clear the input field
    gtk_entry_set_text(GTK_ENTRY(user_input_entry), "");
}

// Function to create the GUI elements
void create_chat_window() {
    // Initialize GTK
    gtk_init(NULL, NULL);

    // Create the main window
    chat_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(chat_window), "Chatbot");
    gtk_window_set_default_size(GTK_WINDOW(chat_window), 400, 500);
    gtk_container_set_border_width(GTK_CONTAINER(chat_window), 10);

    // Create the chat text view (where the conversation will be displayed)
    chat_text_view = gtk_text_view_new();
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_text_view));
    gtk_text_buffer_set_text(buffer, "Chatbot: Hello! How can I assist you today?\n", -1);
    GtkWidget *chat_scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(chat_scroll_window), chat_text_view);
    
    // Create user input entry field
    user_input_entry = gtk_entry_new();

    // Create the "Send" button
    send_button = gtk_button_new_with_label("Send");
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_button_clicked), NULL);

    // Arrange all elements in a vertical box (VBox)
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), chat_scroll_window, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), user_input_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), send_button, FALSE, FALSE, 0);

    // Add the VBox to the window
    gtk_container_add(GTK_CONTAINER(chat_window), vbox);

    // Connect the window "destroy" event to exit GTK
    g_signal_connect(chat_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Show all widgets
    gtk_widget_show_all(chat_window);

    // Start the GTK main loop
    gtk_main();
}

int main(int argc, char *argv[]) {
    // Initialize chatbot with some predefined conversations
    strncpy(conversations[conversation_count].input, "hello", MAX_INPUT_LENGTH);
    strncpy(conversations[conversation_count].response, "Hi there! How can I help you?", MAX_RESPONSE_LENGTH);
    conversation_count++;

    strncpy(conversations[conversation_count].input, "how are you", MAX_INPUT_LENGTH);
    strncpy(conversations[conversation_count].response, "I'm just a bot, but I'm doing fine, thank you!", MAX_RESPONSE_LENGTH);
    conversation_count++;

    strncpy(conversations[conversation_count].input, "your name", MAX_INPUT_LENGTH);
    strncpy(conversations[conversation_count].response, "I am a chatbot, created to assist you!", MAX_RESPONSE_LENGTH);
    conversation_count++;

    // Create and run the chatbot window
    create_chat_window();

    return 0;
}

