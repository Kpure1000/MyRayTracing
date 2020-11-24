#include"render/RenderWindow.h"
#include"util/Color.h"

namespace rtx
{
	namespace render
	{

		void processInput(GLFWwindow* window);

		RenderWindow::RenderWindow(int w, int h, const std::string& title)
			:width(w), height(h), m_title(title)
		{

#pragma region some initializations

            glfwInit(); // 初始化glfw
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#pragma endregion

#pragma region create window

            window = glfwCreateWindow(width, height, m_title.c_str(), NULL, NULL);
            if (window == NULL)
            {
                std::cerr << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
            }
            else
            {
                glfwMakeContextCurrent(window);

                if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
                {
                    std::cerr << "Failed to initialize GLAD" << std::endl;
                }
                else
                {
                    glViewport(0, 0, width, height); // 设置视口大小

                //  window resize call back
                    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
                        {
                            glViewport(0, 0, width, height);
                        });

                    //  mouse scroll event call back
                    glfwSetScrollCallback(window, [](GLFWwindow* win, double offsetX, double offsetY)
                        {
                            // TODO
                            //printf("Offset: (%lf, %lf)\n", offsetX, offsetY);
                        });


#pragma region triangle initialization

                    //  create triangle
                    float vertices[] = {
                        //  triangle down
                        -1.0f,-1.0f,0.0f, //  left-bottom
                        0.5f,0.5f,0.5f,
                        0.0f,0.0f,

                        -1.0f,1.0f,0.0f, //  left-top
                        1.0f,0.0f,0.0f,
                        0.0f,1.0f,

                        1.0f,1.0f,0.0f, //  right-top
                        0.0f,1.0f,0.0f,
                        1.0f,1.0f,

                        1.0f,-1.0f,0.0f, //  right-bottom
                        0.0f,0.0f,1.0f,
                        1.0f,0.0f
                    };

                    int indices[] = {
                        0,1,2,
                        2,3,0
                    };

                    //  create vertices array object
                    glGenVertexArrays(1, &VAO);
                    //  bind vao
                    glBindVertexArray(VAO);

                    //  cerate vertices buffer object
                    glGenBuffers(1, &VBO);
                    //  bind vbo
                    glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    //  copy vertices data to memory
                    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                    glGenBuffers(1, &EBO);
                    //  bind ebo
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                    //  copy element indices data to memory
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

#pragma endregion

#pragma region set vertex atrribution

                    //  set the atrribution of vertices.position data
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
                    //  apply the atrribution set before
                    glEnableVertexAttribArray(0);

                    //  set the atrribution of vertices.color data
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
                    //  apply the atrribution set before
                    glEnableVertexAttribArray(1);

                    //  set the atrribution of vertices.color data
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
                    //  apply the atrribution set before
                    glEnableVertexAttribArray(2);

#pragma endregion

#pragma region texture

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                    glGenTextures(1, &texture);

                    glBindTexture(GL_TEXTURE_2D, texture);

#pragma endregion

#pragma region some setting or source releasing before render

                    //  release bind of vbo
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    //  release bind of vao
                    glBindVertexArray(0);
                    //  release bind of ebo
                    //  <ATTENTION!!!> DO NOT RELEASE EBO BEFORE VAO 
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                    //  set draw mode
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //  draw fill face
                    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //  only draw line
                    //glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); //  only draw vertices

#pragma endregion

                    shader = new render::Shader("../data/shader/ch1_texture.vert",
                        "../data/shader/ch1_texture.frag");

                }
            }


#pragma endregion

		}


		RenderWindow::~RenderWindow()
		{
		}

		void RenderWindow::Run()
		{



#pragma region render loop

            util::Color color;
            float timeValue;

            bool isImage1 = true;

            time_t startTime = clock();
            time_t curTime = startTime;
            int i = 0;
            //  if window has not been closed yet
            while (!glfwWindowShouldClose(window))
            {
                //  deal with input
                processInput(window);

                //  do redner
                glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                //  ready some values for shader
                /*timeValue = glfwGetTime();
                color.g() = sin(2.0f * timeValue) / 2.0f + 0.5f;

                shader.SetColor("verColor", color);*/
                //  use shader to vertices (refresh)
                shader->Use();

                //  bind vao
                glBindVertexArray(VAO);

                glBindTexture(GL_TEXTURE_2D, texture);

                renderTarget.Use();

                //  draw vertices from memory        
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                //  binary buffer swaping
                glfwSwapBuffers(window);
                //  deal events
                glfwPollEvents();
            }

#pragma endregion

#pragma region release all resouces of gl

            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glfwTerminate();

#pragma endregion

		}

		void processInput(GLFWwindow* window)
		{
			//  press ESC to close window and exit
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window, true);
		}
	}
}