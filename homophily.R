library(plotly)

library(ggplot2)
Book3 <- read.csv("C:/Users/mlchen/Desktop/peer/optresult/to-do/Book3.csv")
dt1 <- Book3 %>% filter(u0==0.1)
dt5 <- Book3 %>% filter(u0==0.5)
dt9 <- Book3 %>% filter(u0==0.9)

#homophily
ggplot(h0, aes(x = u1, y = u0, fill = ave)) +
  geom_tile() +
  scale_fill_distiller(palette = "RdYlBu", limits = c(0, 1)) +
  labs(x = "group2 pressure", y = "group1", fill = "fractions")  +
  theme_minimal()+  # Increase the axis title font size
  ggtitle("homophily=0.0")+
  theme(plot.title = element_text(hjust = 0.5, size = 20),  # Increase the plot title font size
        axis.title = element_text(size = 18),  # Increase the axis title font size
        axis.text = element_text(size = 15),  # Increase the axis tick label font size
        panel.border = element_blank(),
        panel.grid.major = element_blank(),
        panel.grid.minor = element_blank())

ggplot(dt1, aes(x = u1, y = h, fill = average)) +
  geom_tile() +
  scale_fill_distiller(palette = "RdYlBu", limits = c(0, 0.6)) +
  labs(x = "group2 pressure", y = "homophily", fill = "fractions")  +
  theme_minimal()+  # Increase the axis title font size
  ggtitle("group1 pressure=0.1")+
  theme(plot.title = element_text(hjust = 0.5, size = 20),  # Increase the plot title font size
        axis.title = element_text(size = 18),  # Increase the axis title font size
        axis.text = element_text(size = 15),  # Increase the axis tick label font size
        panel.border = element_blank(),
        panel.grid.major = element_blank(),
        panel.grid.minor = element_blank())

ggplot(dt5, aes(x = u1, y = h, fill = average)) +
  geom_tile() +
  scale_fill_distiller(palette = "RdYlBu", limits = c(0, 0.6)) +
  labs(x = "group2 pressure", y = "homophily", fill = "fractions")  +
  theme_minimal()+  # Increase the axis title font size
  ggtitle("group1 pressure=0.5")+
  theme(plot.title = element_text(hjust = 0.5, size = 20),  # Increase the plot title font size
        axis.title = element_text(size = 18),  # Increase the axis title font size
        axis.text = element_text(size = 15),  # Increase the axis tick label font size
        panel.border = element_blank(),
        panel.grid.major = element_blank(),
        panel.grid.minor = element_blank())

ggplot(dt9, aes(x = u1, y = h, fill = average)) +
  geom_tile() +
  scale_fill_distiller(palette = "RdYlBu", limits = c(0, 0.6)) +
  labs(x = "group2 pressure", y = "homophily", fill = "fractions")  +
  theme_minimal()+  # Increase the axis title font size
  ggtitle("group1 pressure=0.9")+
  theme(plot.title = element_text(hjust = 0.5, size = 20),  # Increase the plot title font size
        axis.title = element_text(size = 18),  # Increase the axis title font size
        axis.text = element_text(size = 15),  # Increase the axis tick label font size
        panel.border = element_blank(),
        panel.grid.major = element_blank(),
        panel.grid.minor = element_blank())


# inferno colors
ggplot(dt1, aes(x = u1, y = hom, fill = ave)) +
  geom_tile() +
  scale_fill_gradientn(colors = inferno(100), limits = c(0, 0.6)) + # Use inferno palette
  labs(x = "group2 pressure", y = "homophily", fill = "fractions") +
  theme_minimal() +
  ggtitle("group1 pressure = 0.1") +
  theme(
    plot.title = element_text(hjust = 0.5, size = 20),
    axis.title = element_text(size = 18),
    axis.text = element_text(size = 15),
    panel.border = element_blank(),
    panel.grid.major = element_blank(),
    panel.grid.minor = element_blank()
  )


for (i in 1:20){
  ggplot(u9, aes(x = u1, y = hom, fill = p1)) +
    geom_tile() +
    scale_fill_distiller(palette = "RdYlBu", limits = c(0, 1)) +
    labs(x = "group2 pressure", y = "homophily", fill = "fractions")  +
    theme_minimal()+  # Increase the axis title font size
    ggtitle("group1 pressure=0.9, % of 1")+
    theme(plot.title = element_text(hjust = 0.5, size = 20),  # Increase the plot title font size
          axis.title = element_text(size = 18),  # Increase the axis title font size
          axis.text = element_text(size = 15),  # Increase the axis tick label font size
          panel.border = element_blank(),
          panel.grid.major = element_blank(),
          panel.grid.minor = element_blank())
  
}